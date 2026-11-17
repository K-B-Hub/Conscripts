//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Basic/SpecialForcesIdentity.h"
#include "Characters/CharacterBase.h"
#include "GameMode/BattleGameMode.h"

USpecialForcesIdentity::USpecialForcesIdentity()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesIdentity_Name", "특수부대원");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesIdentity_Description", "주변 3m 내에 다른 아군이 없을 때 회피 +5, 명중 +5");

	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::MoveComplete;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	evasion = 5.f;
	accuracy = 5.f;
}

void USpecialForcesIdentity::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	const UWorld* world = o->GetWorld();
	const ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) return;

	//소지자와 같은 진영을 센다, 자신은 제외
	const bool bShouldApply = gm->CountTeamInRange(o->IsAlly(), o->GetActorLocation(), isolationRange, o) == 0;
	if (bShouldApply == bApplied) return;

	bApplied = bShouldApply;
	o->ApplyPassiveStatDelta(this, bApplied ? 1 : -1);
}
