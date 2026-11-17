//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Skill/SpecialForcesAmmoSaving.h"
#include "Characters/CharacterBase.h"

USpecialForcesAmmoSaving::USpecialForcesAmmoSaving()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesAmmoSaving_Name", "탄약 절약");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesAmmoSaving_Description", "치명타 적중 시 장비를 1개 회복한다.");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterCritical;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;
}

void USpecialForcesAmmoSaving::Execute_AfterCritical(ACharacterBase* target, int32 dealtDamage)
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	//최대치 초과분은 GainBattleResource가 클램프
	const int32 before = o->GetBattleResource();
	o->GainBattleResource(resourceGain);
	if (o->GetBattleResource() != before) PlayPassiveEffect();
}
