//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/High/Skill/SpecialForcesBloodThirst.h"
#include "Characters/CharacterBase.h"

USpecialForcesBloodThirst::USpecialForcesBloodThirst()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesBloodThirst_Name", "피의 갈증");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesBloodThirst_Description", "치명타로 피해를 주면 준 피해의 30%만큼 체력을 회복한다.");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterCritical;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;
}

void USpecialForcesBloodThirst::Execute_AfterCritical(ACharacterBase* target, int32 dealtDamage)
{
	ACharacterBase* o = owner.Get();
	if (!o) return;
	if (o->IsDead() || o->GetHp() >= o->GetMaxHp()) return;

	const int32 amount = FMath::RoundToInt(dealtDamage * lifestealPercent / 100.f);
	if (amount <= 0) return;

	//음수 피해가 곧 회복, 비치명이라 AfterDamage를 다시 부르지 않는다
	o->ReceiveDamage(-amount, false);
	PlayPassiveEffect();
}
