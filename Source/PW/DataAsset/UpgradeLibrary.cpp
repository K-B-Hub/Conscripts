//Fill out your copyright notice in the Description page of Project Settings.

#include "DataAsset/UpgradeLibrary.h"
#include "DataAsset/UpgradeTableData.h"
#include "DataAsset/FixedUpgradeTableData.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "ActorComponent/SkillComponent.h"
#include "ActorComponent/PassiveSkillComponent.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "Object/Rest/RestBase.h"
#include "Object/CommonUpgrade/Middle/Rest/ReinforcementRest.h"
#include "GameMode/BattleGameMode.h"
#include "GameInstance/PWGameInstance.h"

TArray<TSubclassOf<USkillBase>> UUpgradeLibrary::BuildPendingChoices(
	const AAllyCharacterBase* character, const UUpgradeTableData* commonTable)
{
	TArray<TSubclassOf<USkillBase>> choices;
	if (!character) return choices;

	//가장 앞 대기 강화의 부여 레벨로 종류를 복원해 분기
	const int32 pendingLevel = character->PeekPendingUpgradeLevel();

	switch (ClassifyLevelUpUpgrade(pendingLevel))
	{
	case ELevelUpUpgradeKind::ClassFixed:
	{
		//직업별 레벨 고정 강화, 단일 카드로 제시
		const UFixedUpgradeTableData* fixedTable = character->GetFixedUpgradeTable();
		const TSubclassOf<USkillBase> fixed = fixedTable ? fixedTable->GetFixedUpgrade(pendingLevel) : nullptr;
		if (fixed) choices.Add(fixed);
		break;
	}
	case ELevelUpUpgradeKind::HighRandom:
		//하급 풀 제외 고급 랜덤
		choices = BuildChoices(character, character->GetClassUpgradeTable(), commonTable, 3, EUpgradeGrade::Mid);
		break;

	default:
		choices = BuildChoices(character, character->GetClassUpgradeTable(), commonTable, 3);
		break;
	}

	return choices;
}

ELevelUpUpgradeKind UUpgradeLibrary::ClassifyLevelUpUpgrade(int32 level)
{
	if (level <= 80 && level % 5 == 0)
	{
		return ELevelUpUpgradeKind::ClassFixed;
	}
	if (level > 80 && level % 10 == 0)
	{
		return ELevelUpUpgradeKind::HighRandom;
	}
	return ELevelUpUpgradeKind::Random;
}

EUpgradeGrade UUpgradeLibrary::RollGrade()
{
	const int32 r = FMath::RandRange(1, 100);
	if (r <= 55) return EUpgradeGrade::Low;		//55%
	if (r <= 85) return EUpgradeGrade::Mid;		//30%
	if (r <= 95) return EUpgradeGrade::High;	//10%
	return EUpgradeGrade::Top;					//5%
}

bool UUpgradeLibrary::CanAcquire(const ACharacterBase* character, TSubclassOf<USkillBase> skillClass)
{
	if (!character || !skillClass) return false;

	//액티브 스킬은 1회만 습득 가능
	if (skillClass->IsChildOf(UActiveSkillBase::StaticClass()))
	{
		const USkillComponent* sc = character->GetSkillComponent();
		const bool bHas = sc && sc->HasSkillClass(skillClass);
		if (bHas)
		{
			UE_LOG(LogTemp, Log, TEXT("[Upgrade] 제외(액티브 중복): %s"), *skillClass->GetName());
		}
		return sc && !bHas;
	}

	//1회성 즉시 효과는 보유 개념이 없어 항상 습득 가능
	if (skillClass->IsChildOf(URestBase::StaticClass()))
	{
		//증원만은 예외, 세울 자리가 없으면 발동해도 허사라 후보에서 뺀다
		if (skillClass->IsChildOf(UReinforcementRest::StaticClass()))
		{
			return CanReinforce(character);
		}
		return true;
	}

	//패시브는 bAllowDuplicate이면 항상 가능, 아니면 미보유 시에만
	if (skillClass->IsChildOf(UPassiveSkillBase::StaticClass()))
	{
		if (skillClass->GetDefaultObject<UPassiveSkillBase>()->bAllowDuplicate) return true;

		const UPassiveSkillComponent* pc = character->GetPassiveSkillComponent();
		const bool bHas = pc && pc->HasPassiveClass(TSubclassOf<UPassiveSkillBase>(skillClass));
		if (bHas)
		{
			UE_LOG(LogTemp, Log, TEXT("[Upgrade] 제외(패시브 중복불가 보유): %s"), *skillClass->GetName());
		}
		return pc && !bHas;
	}

	return false;
}

bool UUpgradeLibrary::CanReinforce(const ACharacterBase* character)
{
	const UWorld* world = character ? character->GetWorld() : nullptr;
	if (!world) return false;

	//야영지에는 ABattleGameMode가 없어 자연히 제외된다, 증원은 전투 전용이다
	const ABattleGameMode* battle = world->GetAuthGameMode<ABattleGameMode>();
	if (!battle) return false;

	const UPWGameInstance* gameInstance = world->GetGameInstance<UPWGameInstance>();
	if (!gameInstance) return false;

	const bool bRoomLeft = battle->GetAllies().Num() < gameInstance->GetMaxRosterSize();
	if (!bRoomLeft)
	{
		UE_LOG(LogTemp, Log, TEXT("[Upgrade] 제외(로스터 상한): 증원"));
	}
	return bRoomLeft;
}

void UUpgradeLibrary::CollectAcquirable(
	const ACharacterBase* character,
	const TArray<TSubclassOf<USkillBase>>& pool,
	const TArray<TSubclassOf<USkillBase>>& already,
	TArray<TSubclassOf<USkillBase>>& outCandidates)
{
	for (const TSubclassOf<USkillBase>& skillClass : pool)
	{
		if (!skillClass) continue;
		//이미 이번 추첨에 뽑힌 것 제외
		if (already.Contains(skillClass) || outCandidates.Contains(skillClass)) continue;
		if (!CanAcquire(character, skillClass)) continue;

		outCandidates.Add(skillClass);
	}
}

void UUpgradeLibrary::PickRandom(TArray<TSubclassOf<USkillBase>>& candidates, int32 n, TArray<TSubclassOf<USkillBase>>& out)
{
	const int32 pickCount = FMath::Min(n, candidates.Num());
	for (int32 i = 0; i < pickCount; ++i)
	{
		const int32 idx = FMath::RandRange(0, candidates.Num() - 1);
		out.Add(candidates[idx]);
		candidates.RemoveAtSwap(idx);
	}
}

TArray<TSubclassOf<USkillBase>> UUpgradeLibrary::BuildChoices(
	const ACharacterBase* character,
	const UUpgradeTableData* classTable,
	const UUpgradeTableData* commonTable,
	int32 count,
	EUpgradeGrade minGrade)
{
	TArray<TSubclassOf<USkillBase>> result;
	if (!character || count <= 0) return result;

	//뽑힌 등급부터 하위 등급까지 내려가며 count를 채움, minGrade 미만은 배제
	const EUpgradeGrade rolledRaw = RollGrade();
	const EUpgradeGrade rolled = (rolledRaw < minGrade) ? minGrade : rolledRaw;
	UE_LOG(LogTemp, Log, TEXT("[Upgrade] %s 강화 추첨 시작 → 등급: %s (요청 %d개, 하한 %s)"),
		*character->GetName(), *UEnum::GetValueAsString(rolled), count, *UEnum::GetValueAsString(minGrade));

	for (int32 g = static_cast<int32>(rolled); g >= static_cast<int32>(minGrade) && result.Num() < count; --g)
	{
		const EUpgradeGrade grade = static_cast<EUpgradeGrade>(g);

		//이 등급의 공용/직업 습득 가능 후보 수집
		TArray<TSubclassOf<USkillBase>> commonCand;
		TArray<TSubclassOf<USkillBase>> classCand;
		if (commonTable) CollectAcquirable(character, commonTable->GetPool(grade), result, commonCand);
		if (classTable)  CollectAcquirable(character, classTable->GetPool(grade), result, classCand);

		const int32 need = count - result.Num();

		//공용/직업 분배를 랜덤 결정, 한쪽이 부족하면 반대쪽에서 보충되도록 범위 클램프
		const int32 maxFromCommon = FMath::Min(need, commonCand.Num());
		const int32 minFromCommon = FMath::Max(0, need - classCand.Num());
		const int32 fromCommon = (minFromCommon <= maxFromCommon) ? FMath::RandRange(minFromCommon, maxFromCommon) : 0;
		const int32 fromClass = need - fromCommon;

		//등급이 뽑힌 등급보다 낮으면 폴백 보충 상황
		UE_LOG(LogTemp, Log, TEXT("[Upgrade]  %s%s 등급: 후보 공용 %d/직업 %d, 필요 %d → 공용 %d + 직업 %d 추출"),
			(grade != rolled) ? TEXT("[폴백] ") : TEXT(""),
			*UEnum::GetValueAsString(grade), commonCand.Num(), classCand.Num(), need, fromCommon, fromClass);

		PickRandom(commonCand, fromCommon, result);
		PickRandom(classCand, fromClass, result);
	}

	//최종 선택지 목록 로그
	FString picked;
	for (const TSubclassOf<USkillBase>& c : result)
	{
		picked += FString::Printf(TEXT("%s "), c ? *c->GetName() : TEXT("null"));
	}
	UE_LOG(LogTemp, Log, TEXT("[Upgrade] 최종 후보 %d개: %s"), result.Num(), *picked);

	return result;
}
