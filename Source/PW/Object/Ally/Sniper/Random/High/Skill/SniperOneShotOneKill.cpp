//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/High/Skill/SniperOneShotOneKill.h"
#include "Characters/CharacterBase.h"

USniperOneShotOneKill::USniperOneShotOneKill()
{
	skillName = NSLOCTEXT("Skill", "SniperOneShotOneKill_Name", "원샷 원킬");
	skillDescription = NSLOCTEXT("Skill", "SniperOneShotOneKill_Description", "체력이 50% 이하인 적 원거리 공격 시 주는 피해 +25%, 치명 +15");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	damageAmplification = 25;
	critical = 15.f;
}

bool USniperOneShotOneKill::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	if (!target) return false;
	ACharacterBase* o = owner.Get();
	if (!o) return false;

	//아군 대상 스킬은 제외
	if (o->IsAlly() == target->IsAlly()) return false;

	return skillType == ESkillType::Ranged && target->GetHp() * 2 <= target->GetMaxHp();
}
