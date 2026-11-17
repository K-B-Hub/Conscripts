//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Top/Skill/SpecialForcesShadowCounter.h"
#include "Characters/CharacterBase.h"

USpecialForcesShadowCounter::USpecialForcesShadowCounter()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesShadowCounter_Name", "그림자 반격");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesShadowCounter_Description", "공격을 회피하면 공격자에게 공격력 × 1.0의 반격 피해를 준다(명중, 치명 판정 없음, 체력을 1 아래로 깎지 않음).");
	skillGrade = EUpgradeGrade::Top;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterEvade;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;
}

void USpecialForcesShadowCounter::Execute_AfterEvade(ACharacterBase* attacker)
{
	if (!attacker || attacker->IsDead()) return;
	ACharacterBase* o = owner.Get();
	if (!o) return;
	if (o->IsDead()) return;

	//일반 피해 산식에서 명중·치명만 뺀 값, 피해 증폭·관통·방어·받는 피해 감소는 그대로
	const float raw = o->GetAtk() * counterRatio * (1.f + o->GetDamageAmplification() / 100.f)
		- attacker->GetDef() * (1.f - o->GetPenetration() / 100.f);
	const int32 amount = FMath::RoundToInt(FMath::Max(0.f, raw) * (1.f - attacker->GetDamageReduction() / 100.f));
	if (amount <= 0) return;

	//비치명이라 적 행동 도중 공격자가 죽지 않고 AfterDamage도 다시 부르지 않는다
	attacker->ReceiveDamage(amount, false);
	PlayPassiveEffect();
}
