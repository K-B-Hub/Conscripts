//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/CommonUpgrade/Low/LowEvasionPassive.h"

ULowEvasionPassive::ULowEvasionPassive()
{
	skillName = NSLOCTEXT("Skill", "LowEvasion_Name", "하급 회피 증가");
	skillDescription = NSLOCTEXT("Skill", "LowEvasion_Description", "회피가 3 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	evasion = 3.0f;
}
