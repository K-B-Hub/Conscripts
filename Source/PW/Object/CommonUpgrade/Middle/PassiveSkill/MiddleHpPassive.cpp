//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Middle/PassiveSkill/MiddleHpPassive.h"

UMiddleHpPassive::UMiddleHpPassive()
{
	skillName = NSLOCTEXT("Skill", "MiddleHp_Name", "중급 체력 증가");
	skillDescription = NSLOCTEXT("Skill", "MiddleHp_Description", "최대 체력이 3 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	hp = 3;
}
