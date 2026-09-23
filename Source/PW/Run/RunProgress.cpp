//Fill out your copyright notice in the Description page of Project Settings.

#include "Run/RunProgress.h"
#include "Characters/AllyCharacterBase.h"
#include "DataAsset/RunModeData.h"

namespace
{
	//예고 화면이 다음과 그 다음 두 칸을 보여주므로 그만큼 미리 뽑아 둔다
	constexpr int32 LookaheadCount = 2;
}

void URunProgress::StartFixedRun(const TArray<FAllyRunState>& initialRoster, FName inStoryRouteId, const TArray<FStageEntry>& inStages)
{
	roster = initialRoster;
	stages = inStages;
	stageIndex = 0;
	bRunActive = true;
	storyRouteId = inStoryRouteId;
}

void URunProgress::StartGeneratedRun(const TArray<FAllyRunState>& initialRoster, URunModeData* inModeData)
{
	roster = initialRoster;
	stageIndex = 0;
	bRunActive = true;
	storyRouteId = NAME_None;
	modeData = inModeData;
	campVisitsLeft = inModeData ? inModeData->playerCampVisits : 0;

	//첫 스테이지는 추첨 없이 전투다
	stages.Reset();
	if (!inModeData) return;

	stages.Add(PickFromPool(inModeData->battlePool));
	EnsureLookahead();
}

const FStageEntry* URunProgress::GetCurrentStage() const
{
	return GetUpcomingStage(0);
}

const FStageEntry* URunProgress::GetUpcomingStage(int32 offset) const
{
	const int32 index = stageIndex + offset;
	return stages.IsValidIndex(index) ? &stages[index] : nullptr;
}

int32 URunProgress::GetTotalBattles() const
{
	//고정 시퀀스는 목표치가 따로 없으므로 배열에 든 전투 수가 곧 목표다
	if (modeData) return modeData->totalBattles;

	int32 count = 0;
	for (const FStageEntry& stage : stages)
	{
		if (stage.Type == EStageType::Battle) ++count;
	}
	return count;
}

bool URunProgress::AdvanceStage()
{
	const FStageEntry* current = GetCurrentStage();
	const bool bWasBattle = current && current->Type == EStageType::Battle;

	if (bWasBattle) ++battlesCleared;

	++stageIndex;

	//고정 시퀀스는 배열이 끝나면 런도 끝난다
	if (!modeData) return stages.IsValidIndex(stageIndex);

	//야영지가 전투 수에 들지 않으므로 배열 길이로는 완주를 판정할 수 없다
	if (battlesCleared >= modeData->totalBattles) return false;

	EnsureLookahead();
	return stages.IsValidIndex(stageIndex);
}

void URunProgress::EnsureLookahead()
{
	if (!modeData) return;

	while (stages.Num() < stageIndex + LookaheadCount)
	{
		//예고에 잡힌 전투까지 세어, 목표를 넘는 전투를 미리 보여주지 않는다
		if (battlesCleared + CountPendingBattles() >= modeData->totalBattles) return;

		//마지막으로 정해진 스테이지가 전투였는지가 야영지 추첨의 조건이다
		const bool bAfterBattle = stages.Num() > 0 && stages.Last().Type == EStageType::Battle;
		stages.Add(RollNextStage(bAfterBattle));
	}
}

int32 URunProgress::CountPendingBattles() const
{
	int32 count = 0;
	for (int32 i = stageIndex; i < stages.Num(); ++i)
	{
		if (stages[i].Type == EStageType::Battle) ++count;
	}
	return count;
}

FStageEntry URunProgress::RollNextStage(bool bAfterBattle)
{
	if (bAfterBattle && modeData->campPool.Num() > 0)
	{
		const float chance = modeData->campChanceBase + campSkipStreak * modeData->campChanceStep;
		if (FMath::FRand() < chance)
		{
			campSkipStreak = 0;
			return PickFromPool(modeData->campPool);
		}

		++campSkipStreak;
	}

	return PickFromPool(modeData->battlePool);
}

FStageEntry URunProgress::PickFromPool(const TArray<FStageEntry>& pool) const
{
	if (pool.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[RunProgress] 스테이지 후보 풀이 비어 있습니다"));
		return FStageEntry();
	}

	return pool[FMath::RandRange(0, pool.Num() - 1)];
}

bool URunProgress::InsertCampVisit()
{
	if (campVisitsLeft <= 0 || !modeData || modeData->campPool.Num() == 0) return false;

	//예고해 둔 다음 스테이지는 야영지 뒤로 밀린다
	stages.Insert(PickFromPool(modeData->campPool), stageIndex);
	--campVisitsLeft;
	return true;
}

void URunProgress::EndRun()
{
	bRunActive = false;
}

void URunProgress::AddToRoster(const FAllyRunState& newMember)
{
	if (!newMember.IsValid()) return;

	roster.Add(newMember);
}

void URunProgress::RestoreToWorld(const TArray<AAllyCharacterBase*>& allies) const
{
	//스폰 측이 로스터 순서대로 만들었다는 전제, 개수가 어긋나면 스폰 로직 결함이므로 로그로 드러낸다
	if (allies.Num() != roster.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("[RunProgress] 스폰 인원(%d)과 로스터(%d) 불일치"),
			allies.Num(), roster.Num());
	}

	const int32 count = FMath::Min(allies.Num(), roster.Num());
	for (int32 i = 0; i < count; ++i)
	{
		if (!allies[i]) continue;

		allies[i]->RestoreRunState(roster[i]);
	}
}

void URunProgress::CaptureFromWorld(const TArray<AAllyCharacterBase*>& allies)
{
	//전원 출격이므로 살아 돌아온 캐릭터들의 스냅샷이 곧 새 로스터가 되고, 사망자는 자연히 빠진다
	TArray<FAllyRunState> survivors;
	survivors.Reserve(allies.Num());

	for (const AAllyCharacterBase* ally : allies)
	{
		if (!IsValid(ally) || ally->IsDead()) continue;

		survivors.Add(ally->CaptureRunState());
	}

	roster = MoveTemp(survivors);
}