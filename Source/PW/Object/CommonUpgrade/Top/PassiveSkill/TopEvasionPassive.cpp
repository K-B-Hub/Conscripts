//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Top/PassiveSkill/TopEvasionPassive.h"

UTopEvasionPassive::UTopEvasionPassive()
{
	skillName = NSLOCTEXT("Skill", "TopEvasion_Name", "민첩한 움직임");
	skillDescription = NSLOCTEXT("Skill", "TopEvasion_Description", "회피가 35 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	evasion = 35.0f;
}