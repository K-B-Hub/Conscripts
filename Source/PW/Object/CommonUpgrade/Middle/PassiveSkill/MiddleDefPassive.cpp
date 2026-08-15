//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Middle/PassiveSkill/MiddleDefPassive.h"

UMiddleDefPassive::UMiddleDefPassive()
{
	skillName = NSLOCTEXT("Skill", "MiddleDef_Name", "중급 방어력 증가");
	skillDescription = NSLOCTEXT("Skill", "MiddleDef_Description", "방어력이 2 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	def = 2;
}
