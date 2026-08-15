//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Low/PassiveSkill/LowCriticalPassive.h"

ULowCriticalPassive::ULowCriticalPassive()
{
	skillName = NSLOCTEXT("Skill", "LowCritical_Name", "하급 치명타 증가");
	skillDescription = NSLOCTEXT("Skill", "LowCritical_Description", "치명타가 3 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	critical = 3.0f;
}
