//Fill out your copyright notice in the Description page of Project Settings.

#include "Run/RunProgress.h"
#include "Characters/AllyCharacterBase.h"

void URunProgress::StartRun(const TArray<FAllyRunState>& initialRoster, FName inStoryRouteId, const TArray<FStageEntry>& inStages)
{
	roster = initialRoster;
	stages = inStages;
	stageIndex = 0;
	bRunActive = true;
	storyRouteId = inStoryRouteId;
}

const FStageEntry* URunProgress::GetCurrentStage() const
{
	return stages.IsValidIndex(stageIndex) ? &stages[stageIndex] : nullptr;
}

bool URunProgress::AdvanceStage()
{
	++stageIndex;
	return stages.IsValidIndex(stageIndex);
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