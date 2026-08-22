//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/High/PassiveSkill/HighMentalityPassive.h"

UHighMentalityPassive::UHighMentalityPassive()
{
	skillName = NSLOCTEXT("Skill", "HighMentality_Name", "상급 정신력 증가");
	skillDescription = NSLOCTEXT("Skill", "HighMentality_Description", "정신력이 3 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	mentality = 3;
}
