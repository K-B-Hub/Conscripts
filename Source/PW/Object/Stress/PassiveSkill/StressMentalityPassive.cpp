//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Stress/PassiveSkill/StressMentalityPassive.h"

UStressMentalityPassive::UStressMentalityPassive()
{
	skillName = NSLOCTEXT("Skill", "StressMentalityPassive_Name", "극복");
	skillDescription = NSLOCTEXT("Skill", "StressMentalityPassive_Description", "극한의 상황을 이겨내 정신력이 10 증가한다.");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	bAllowDuplicate = true;

	mentality = 10;
}