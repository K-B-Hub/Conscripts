//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Unused/Skill/SniperExpert.h"

USniperExpert::USniperExpert()
{
	skillName = NSLOCTEXT("Skill", "SniperExpert_Name", "저격 전문가");
	skillDescription = NSLOCTEXT("Skill", "SniperExpert_Description", "주는 피해 +10%");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	bAllowDuplicate = false;

	damageAmplification = 10;
}
