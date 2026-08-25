//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Top/PassiveSkill/TopPenetrationPassive.h"

UTopPenetrationPassive::UTopPenetrationPassive()
{
	skillName = NSLOCTEXT("Skill", "TopPenetration_Name", "강선 강화");
	skillDescription = NSLOCTEXT("Skill", "TopPenetration_Description", "관통력이 25 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	penetration = 25;
}