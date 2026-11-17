//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/High/Skill/SniperGlassCannon.h"

USniperGlassCannon::USniperGlassCannon()
{
	skillName = NSLOCTEXT("Skill", "SniperGlassCannon_Name", "유리 대포");
	skillDescription = NSLOCTEXT("Skill", "SniperGlassCannon_Description", "주는 피해 +30%, 받는 피해 +30%");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	bAllowDuplicate = false;

	damageAmplification = 30;
	damageReduction = -30;
}
