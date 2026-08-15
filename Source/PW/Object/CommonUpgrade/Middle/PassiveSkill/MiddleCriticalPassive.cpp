//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Middle/PassiveSkill/MiddleCriticalPassive.h"

UMiddleCriticalPassive::UMiddleCriticalPassive()
{
	skillName = NSLOCTEXT("Skill", "MiddleCritical_Name", "중급 치명타 증가");
	skillDescription = NSLOCTEXT("Skill", "MiddleCritical_Description", "치명타가 5 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	critical = 5.0f;
}
