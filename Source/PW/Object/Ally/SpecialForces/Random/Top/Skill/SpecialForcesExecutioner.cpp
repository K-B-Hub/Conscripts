//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Top/Skill/SpecialForcesExecutioner.h"
#include "Characters/CharacterBase.h"
#include "GameMode/BattleGameMode.h"

USpecialForcesExecutioner::USpecialForcesExecutioner()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesExecutioner_Name", "처형자");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesExecutioner_Description", "치명타 적중 시 행동력 1 회복(라운드당 1회)");
	skillGrade = EUpgradeGrade::Top;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterCritical;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;
}

void USpecialForcesExecutioner::Execute_AfterCritical(ACharacterBase* target, int32 dealtDamage)
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	const UWorld* world = o->GetWorld();
	const ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) return;

	const int32 currentRound = gm->GetCurrentRound();
	if (currentRound == triggeredRound) return;

	//음수 차감이 곧 회복, 최대치라 못 받았으면 이번 라운드 기회를 쓰지 않는다
	const int32 before = o->GetCurrentActionPoint();
	o->ReduceActionPoint(-actionPointGain);
	if (o->GetCurrentActionPoint() == before) return;

	triggeredRound = currentRound;
	PlayPassiveEffect();
}
