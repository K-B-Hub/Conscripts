//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Middle/PassiveSkill/MiddleMentalityPassive.h"

UMiddleMentalityPassive::UMiddleMentalityPassive()
{
	skillName = NSLOCTEXT("Skill", "MiddleMentality_Name", "중급 정신력 증가");
	skillDescription = NSLOCTEXT("Skill", "MiddleMentality_Description", "정신력이 2 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	mentality = 2;
}
