//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Middle/Skill/SniperAmmoBelt.h"

USniperAmmoBelt::USniperAmmoBelt()
{
	skillName = NSLOCTEXT("Skill", "SniperAmmoBelt_Name", "탄띠 확장");
	skillDescription = NSLOCTEXT("Skill", "SniperAmmoBelt_Description", "탄환 최대치 +5");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	bAllowDuplicate = false;

	maxBattleResource = 5;
}
