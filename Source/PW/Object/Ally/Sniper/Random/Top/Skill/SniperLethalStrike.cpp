//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Top/Skill/SniperLethalStrike.h"

USniperLethalStrike::USniperLethalStrike()
{
	skillName = NSLOCTEXT("Skill", "SniperLethalStrike_Name", "치명적인 공격");
	skillDescription = NSLOCTEXT("Skill", "SniperLethalStrike_Description", "치명타 판정에 실패하면 한 번 더 판정한다.");
	skillGrade = EUpgradeGrade::Top;

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	bAllowDuplicate = false;

	bExtraCriticalRoll = true;
}
