//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Middle/PassiveSkill/MiddleSpeedPassive.h"

UMiddleSpeedPassive::UMiddleSpeedPassive()
{
	skillName = NSLOCTEXT("Skill", "MiddleSpeed_Name", "중급 속도 증가");
	skillDescription = NSLOCTEXT("Skill", "MiddleSpeed_Description", "속도가 2 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	speed = 2;
}
