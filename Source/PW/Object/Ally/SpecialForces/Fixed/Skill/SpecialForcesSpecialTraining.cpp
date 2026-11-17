//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Fixed/Skill/SpecialForcesSpecialTraining.h"

USpecialForcesSpecialTraining::USpecialForcesSpecialTraining()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesSpecialTraining_Name", "특수 훈련");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesSpecialTraining_Description", "회피 +10, 명중 +10, 장비 최대치 +2");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	bAllowDuplicate = false;

	evasion = 10.f;
	accuracy = 10.f;
	maxBattleResource = 2;
}
