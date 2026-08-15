//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Low/PassiveSkill/LowMentalityPassive.h"

ULowMentalityPassive::ULowMentalityPassive()
{
	skillName = NSLOCTEXT("Skill", "LowMentality_Name", "하급 정신력 증가");
	skillDescription = NSLOCTEXT("Skill", "LowMentality_Description", "정신력이 1 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	mentality = 1;
}
