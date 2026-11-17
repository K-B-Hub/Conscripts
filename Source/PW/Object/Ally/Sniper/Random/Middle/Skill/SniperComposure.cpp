//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Middle/Skill/SniperComposure.h"
#include "Characters/CharacterBase.h"

USniperComposure::USniperComposure()
{
	skillName = NSLOCTEXT("Skill", "SniperComposure_Name", "냉정함");
	skillDescription = NSLOCTEXT("Skill", "SniperComposure_Description", "자신의 체력이 50% 이상일 때 공격 시 명중 +10");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	accuracy = 10.f;
}

bool USniperComposure::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	if (!target) return false;
	ACharacterBase* o = owner.Get();
	if (!o) return false;

	//아군 대상 스킬은 제외
	if (o->IsAlly() == target->IsAlly()) return false;

	return o->GetHp() * 2 >= o->GetMaxHp();
}
