//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Middle/PassiveSkill/MiddleAccuracyPassive.h"

UMiddleAccuracyPassive::UMiddleAccuracyPassive()
{
	skillName = NSLOCTEXT("Skill", "MiddleAccuracy_Name", "중급 명중 증가");
	skillDescription = NSLOCTEXT("Skill", "MiddleAccuracy_Description", "명중이 5 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	accuracy = 5.0f;
}
