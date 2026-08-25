//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/High/PassiveSkill/HighEndurancePassive.h"
#include "Characters/CharacterBase.h"

UHighEndurancePassive::UHighEndurancePassive()
{
	skillName = NSLOCTEXT("Skill", "HighEndurance_Name", "위기모면");
	skillDescription = NSLOCTEXT("Skill", "HighEndurance_Description", "체력이 30% 이상일 때 치명적인 공격을 받아도 체력 1로 생존한다");

	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::BeforeDeath;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;
}

bool UHighEndurancePassive::Execute_BeforeDeath(int32 hpBefore, int32 incomingDamage)
{
	const ACharacterBase* o = owner.Get();
	if (!o) return false;

	//피격 직전 체력 기준, 정수 비교로 부동소수 오차 회피
	return hpBefore * 100 >= o->GetMaxHp() * hpThresholdPercent;
}