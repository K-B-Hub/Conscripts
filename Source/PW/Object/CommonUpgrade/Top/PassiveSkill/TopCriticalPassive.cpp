//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Top/PassiveSkill/TopCriticalPassive.h"

UTopCriticalPassive::UTopCriticalPassive()
{
	skillName = NSLOCTEXT("Skill", "TopCritical_Name", "예리함");
	skillDescription = NSLOCTEXT("Skill", "TopCritical_Description", "치명타가 30 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	critical = 30.0f;
}