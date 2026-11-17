//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Skill/SpecialForcesLoot.h"
#include "Characters/CharacterBase.h"

USpecialForcesLoot::USpecialForcesLoot()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesLoot_Name", "노획");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesLoot_Description", "적 처치 시 장비를 1개 회복한다.");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterSlay;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;
}

void USpecialForcesLoot::Execute_AfterSlay(FVector slainLocation)
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	//최대치 초과분은 GainBattleResource가 클램프
	const int32 before = o->GetBattleResource();
	o->GainBattleResource(resourceGain);
	if (o->GetBattleResource() != before) PlayPassiveEffect();
}
