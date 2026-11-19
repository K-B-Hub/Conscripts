//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Unused/Skill/SpecialForcesCombatExperience.h"

USpecialForcesCombatExperience::USpecialForcesCombatExperience()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesCombatExperience_Name", "손때 묻은 카빈");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesCombatExperience_Description", "행동력 +1, 장비 최대치 +2");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	bAllowDuplicate = false;

	actionPoint = 1;
	maxBattleResource = 2;
}
