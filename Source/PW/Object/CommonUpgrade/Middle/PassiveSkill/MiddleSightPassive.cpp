//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Middle/PassiveSkill/MiddleSightPassive.h"

UMiddleSightPassive::UMiddleSightPassive()
{
	skillName = NSLOCTEXT("Skill", "MiddleSight_Name", "중급 시야 증가");
	skillDescription = NSLOCTEXT("Skill", "MiddleSight_Description", "시야가 2 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	sight = 2.0f;
}
