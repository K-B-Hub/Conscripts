//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/High/PassiveSkill/HighAccuracyPassive.h"

UHighAccuracyPassive::UHighAccuracyPassive()
{
	skillName = NSLOCTEXT("Skill", "HighAccuracy_Name", "상급 명중 증가");
	skillDescription = NSLOCTEXT("Skill", "HighAccuracy_Description", "명중이 10 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	accuracy = 10.0f;
}
