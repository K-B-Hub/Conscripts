//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Middle/PassiveSkill/MiddleEvasionPassive.h"

UMiddleEvasionPassive::UMiddleEvasionPassive()
{
	skillName = NSLOCTEXT("Skill", "MiddleEvasion_Name", "중급 회피 증가");
	skillDescription = NSLOCTEXT("Skill", "MiddleEvasion_Description", "회피가 5 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	evasion = 5.0f;
}
