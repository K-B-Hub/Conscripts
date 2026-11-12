//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Fixed/Skill/RifleSkilledShooting.h"

URifleSkilledShooting::URifleSkilledShooting()
{
	skillName = NSLOCTEXT("Skill", "RifleSkilledShooting_Name", "능숙한 사격");
	skillDescription = NSLOCTEXT("Skill", "RifleSkilledShooting_Description", "명중 +10, 치명 +10, 탄창 최대치 +3");

	//조건 없이 상시 적용되는 스탯 강화
	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	bAllowDuplicate = false;

	accuracy = 10.f;
	critical = 10.f;
	maxBattleResource = 3;
}
