//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Top/PassiveSkill/TopSightPassive.h"

UTopSightPassive::UTopSightPassive()
{
	skillName = NSLOCTEXT("Skill", "TopSight_Name", "천리안");
	skillDescription = NSLOCTEXT("Skill", "TopSight_Description", "시야가 20 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	sight = 20.0f;
}