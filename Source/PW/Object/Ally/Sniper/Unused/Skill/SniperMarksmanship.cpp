//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Unused/Skill/SniperMarksmanship.h"

USniperMarksmanship::USniperMarksmanship()
{
	skillName = NSLOCTEXT("Skill", "SniperMarksmanship_Name", "저격 숙련");
	skillDescription = NSLOCTEXT("Skill", "SniperMarksmanship_Description", "명중 +5");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	bAllowDuplicate = false;

	accuracy = 5.f;
}
