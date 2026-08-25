//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/High/PassiveSkill/HighCriticalDamagePassive.h"

UHighCriticalDamagePassive::UHighCriticalDamagePassive()
{
	skillName = NSLOCTEXT("Skill", "HighCriticalDamage_Name", "일격필살");
	skillDescription = NSLOCTEXT("Skill", "HighCriticalDamage_Description", "치명타 피해가 100% 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//배율 가산이라 중복 시 증가폭이 과도해져 1회만 획득 가능
	bAllowDuplicate = false;

	criticalDamage = 1.0f;
}