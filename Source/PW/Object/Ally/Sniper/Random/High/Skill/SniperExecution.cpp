//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/High/Skill/SniperExecution.h"
#include "Characters/CharacterBase.h"

USniperExecution::USniperExecution()
{
	skillName = NSLOCTEXT("Skill", "SniperExecution_Name", "처형");
	skillDescription = NSLOCTEXT("Skill", "SniperExecution_Description", "체력이 30% 이하인 적 공격 시 주는 피해 +100%, 치명 +20");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	damageAmplification = 100;
	critical = 20.f;
}

bool USniperExecution::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	if (!target) return false;
	ACharacterBase* o = owner.Get();
	if (!o) return false;

	//아군 대상 스킬은 제외
	if (o->IsAlly() == target->IsAlly()) return false;

	return target->GetHp() * 10 <= target->GetMaxHp() * 3;
}
