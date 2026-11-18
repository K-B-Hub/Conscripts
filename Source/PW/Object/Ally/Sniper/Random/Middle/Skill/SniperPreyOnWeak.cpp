//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Middle/Skill/SniperPreyOnWeak.h"
#include "Characters/CharacterBase.h"

USniperPreyOnWeak::USniperPreyOnWeak()
{
	skillName = NSLOCTEXT("Skill", "SniperPreyOnWeak_Name", "약자 사냥");
	skillDescription = NSLOCTEXT("Skill", "SniperPreyOnWeak_Description", "체력이 50% 이하인 적 공격 시 치명 +10");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	critical = 10.f;
}

bool USniperPreyOnWeak::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	if (!target) return false;
	ACharacterBase* o = owner.Get();
	if (!o) return false;

	//아군 대상 스킬은 제외
	if (o->IsAlly() == target->IsAlly()) return false;

	return target->GetHp() * 2 <= target->GetMaxHp();
}
