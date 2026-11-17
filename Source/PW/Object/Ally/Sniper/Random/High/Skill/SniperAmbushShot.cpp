//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/High/Skill/SniperAmbushShot.h"
#include "Characters/CharacterBase.h"

USniperAmbushShot::USniperAmbushShot()
{
	skillName = NSLOCTEXT("Skill", "SniperAmbushShot_Name", "기습 사격");
	skillDescription = NSLOCTEXT("Skill", "SniperAmbushShot_Description", "체력이 가득 찬 적 공격 시 주는 피해 +30%");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	damageAmplification = 30;
}

bool USniperAmbushShot::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	if (!target) return false;
	ACharacterBase* o = owner.Get();
	if (!o) return false;

	//아군 대상 스킬은 제외
	if (o->IsAlly() == target->IsAlly()) return false;

	return target->GetHp() >= target->GetMaxHp();
}
