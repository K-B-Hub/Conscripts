//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Middle/PassiveSkill/MiddlePenetrationPassive.h"

UMiddlePenetrationPassive::UMiddlePenetrationPassive()
{
	skillName = NSLOCTEXT("Skill", "MiddlePenetration_Name", "관통력 증가");
	skillDescription = NSLOCTEXT("Skill", "MiddlePenetration_Description", "관통력이 3 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	penetration = 3;
}
