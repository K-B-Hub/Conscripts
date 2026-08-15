//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Middle/PassiveSkill/MiddleAtkPassive.h"

UMiddleAtkPassive::UMiddleAtkPassive()
{
	skillName = NSLOCTEXT("Skill", "MiddleAtk_Name", "중급 공격력 증가");
	skillDescription = NSLOCTEXT("Skill", "MiddleAtk_Description", "공격력이 2 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	atk = 2;
}
