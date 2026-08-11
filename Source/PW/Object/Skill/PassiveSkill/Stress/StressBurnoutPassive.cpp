//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/Stress/StressBurnoutPassive.h"

UStressBurnoutPassive::UStressBurnoutPassive()
{
	skillName = NSLOCTEXT("Skill", "StressBurnoutPassive_Name", "번아웃");
	skillDescription = NSLOCTEXT("Skill", "StressBurnoutPassive_Description", "무리한 끝에 정신이 타버렸다. 정신력이 10 감소한다.");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	bAllowDuplicate = true;

	mentality = -10;
}