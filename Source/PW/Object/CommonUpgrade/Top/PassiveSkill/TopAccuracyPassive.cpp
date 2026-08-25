//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Top/PassiveSkill/TopAccuracyPassive.h"

UTopAccuracyPassive::UTopAccuracyPassive()
{
	skillName = NSLOCTEXT("Skill", "TopAccuracy_Name", "특등 사수");
	skillDescription = NSLOCTEXT("Skill", "TopAccuracy_Description", "명중이 40 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	accuracy = 40.0f;
}