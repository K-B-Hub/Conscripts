//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/High/PassiveSkill/HighPenetrationPassive.h"

UHighPenetrationPassive::UHighPenetrationPassive()
{
	skillName = NSLOCTEXT("Skill", "HighPenetration_Name", "상급 관통력 증가");
	skillDescription = NSLOCTEXT("Skill", "HighPenetration_Description", "관통력이 6 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	penetration = 6;
}
