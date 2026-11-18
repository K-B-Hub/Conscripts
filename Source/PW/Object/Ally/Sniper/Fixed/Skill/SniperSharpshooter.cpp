//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Fixed/Skill/SniperSharpshooter.h"

USniperSharpshooter::USniperSharpshooter()
{
	skillName = NSLOCTEXT("Skill", "SniperSharpshooter_Name", "명사수");
	skillDescription = NSLOCTEXT("Skill", "SniperSharpshooter_Description", "명중 +10, 치명 +5, 탄환 최대치 +2");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	bAllowDuplicate = false;

	accuracy = 10.f;
	critical = 5.f;
	maxBattleResource = 2;
}
