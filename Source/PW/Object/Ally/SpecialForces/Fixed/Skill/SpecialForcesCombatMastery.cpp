//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Fixed/Skill/SpecialForcesCombatMastery.h"

USpecialForcesCombatMastery::USpecialForcesCombatMastery()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesCombatMastery_Name", "전투 숙련");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesCombatMastery_Description", "치명 +10, 치명타 피해 +30%");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	bAllowDuplicate = false;

	critical = 10.f;
	criticalDamage = 0.3f;
}
