//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Skill/SpecialForcesAgility.h"
#include "Characters/CharacterBase.h"
#include "GameMode/BattleGameMode.h"

USpecialForcesAgility::USpecialForcesAgility()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesAgility_Name", "날렵함");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesAgility_Description", "주변 4m 내의 적 1명당 회피 +2(최대 10)");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::MoveComplete;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	evasion = 2.f;
}

void USpecialForcesAgility::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	const UWorld* world = o->GetWorld();
	const ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) return;

	const int32 targetStack = FMath::Min(gm->CountTeamInRange(!o->IsAlly(), o->GetActorLocation(), searchRange), maxStack);
	const int32 delta = targetStack - appliedStack;
	if (delta == 0) return;

	//ApplyPassiveStatDelta는 sign ±1만 받으므로 차이만큼 반복 호출
	const int32 sign = (delta > 0) ? 1 : -1;
	for (int32 i = 0; i < FMath::Abs(delta); ++i)
	{
		o->ApplyPassiveStatDelta(this, sign);
	}
	appliedStack = targetStack;
}
