//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Fixed/RifleCombatExperience.h"

URifleCombatExperience::URifleCombatExperience()
{
	skillName = NSLOCTEXT("Skill", "RifleCombatExperience_Name", "전투 숙련");
	skillDescription = NSLOCTEXT("Skill", "RifleCombatExperience_Description", "행동력이 1, 탄창이 2 증가한다");

	//조건 없이 상시 적용되는 스탯 강화
	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	bAllowDuplicate = false;

	actionPoint = 1;
	maxBattleResource = 2;
}
