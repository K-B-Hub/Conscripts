//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/High/Skill/SniperNest.h"
#include "Characters/CharacterBase.h"

USniperNest::USniperNest()
{
	skillName = NSLOCTEXT("Skill", "SniperNest_Name", "저격 진지");
	skillDescription = NSLOCTEXT("Skill", "SniperNest_Description", "포복 상태에서 공격 시 명중 +10, 치명 +15, 주는 피해 +10%");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	accuracy = 10.f;
	critical = 15.f;
	damageAmplification = 10;
}

bool USniperNest::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	if (!target) return false;
	ACharacterBase* o = owner.Get();
	if (!o) return false;

	//아군 대상 스킬은 제외
	if (o->IsAlly() == target->IsAlly()) return false;

	return o->IsProne();
}
