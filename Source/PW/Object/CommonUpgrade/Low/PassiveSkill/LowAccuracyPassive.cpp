//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Low/PassiveSkill/LowAccuracyPassive.h"

ULowAccuracyPassive::ULowAccuracyPassive()
{
	skillName = NSLOCTEXT("Skill", "LowAccuracy_Name", "하급 명중 증가");
	skillDescription = NSLOCTEXT("Skill", "LowAccuracy_Description", "명중이 3 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	accuracy = 3.0f;
}
