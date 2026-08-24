//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/High/PassiveSkill/HighSpeedPassive.h"

UHighSpeedPassive::UHighSpeedPassive()
{
	skillName = NSLOCTEXT("Skill", "HighSpeed_Name", "상급 속도 증가");
	skillDescription = NSLOCTEXT("Skill", "HighSpeed_Description", "속도가 3 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	speed = 3;
}
