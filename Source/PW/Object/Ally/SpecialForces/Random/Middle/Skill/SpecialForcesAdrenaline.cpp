//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Skill/SpecialForcesAdrenaline.h"
#include "Characters/CharacterBase.h"

USpecialForcesAdrenaline::USpecialForcesAdrenaline()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesAdrenaline_Name", "아드레날린");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesAdrenaline_Description", "공격을 회피하면 최대 체력의 10%를 회복한다.");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterEvade;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;
}

void USpecialForcesAdrenaline::Execute_AfterEvade(ACharacterBase* attacker)
{
	ACharacterBase* o = owner.Get();
	if (!o) return;
	if (o->IsDead() || o->GetHp() >= o->GetMaxHp()) return;

	//음수 피해가 곧 회복, 비치명이라 AfterDamage를 다시 부르지 않는다
	const int32 amount = FMath::Max(1, FMath::RoundToInt(o->GetMaxHp() * healPercent / 100.f));
	o->ReceiveDamage(-amount, false);
	PlayPassiveEffect();
}
