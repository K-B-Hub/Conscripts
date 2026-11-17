//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Middle/Skill/SniperVitalAim.h"

USniperVitalAim::USniperVitalAim()
{
	skillName = NSLOCTEXT("Skill", "SniperVitalAim_Name", "급소 조준");
	skillDescription = NSLOCTEXT("Skill", "SniperVitalAim_Description", "치명 +10");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	bAllowDuplicate = false;

	critical = 10.f;
}
