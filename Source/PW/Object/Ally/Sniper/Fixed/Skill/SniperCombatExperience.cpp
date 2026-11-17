//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Fixed/Skill/SniperCombatExperience.h"

USniperCombatExperience::USniperCombatExperience()
{
	skillName = NSLOCTEXT("Skill", "SniperCombatExperience_Name", "손때 묻은 저격총");
	skillDescription = NSLOCTEXT("Skill", "SniperCombatExperience_Description", "행동력 +1, 탄환 최대치 +2");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	bAllowDuplicate = false;

	actionPoint = 1;
	maxBattleResource = 2;
}
