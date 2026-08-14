//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/CommonUpgrade/Low/LowAtkPassive.h"

ULowAtkPassive::ULowAtkPassive()
{
	skillName = NSLOCTEXT("Skill", "LowAtk_Name", "하급 공격력 증가");
	skillDescription = NSLOCTEXT("Skill", "LowAtk_Description", "공격력이 1 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	atk = 1;
}
