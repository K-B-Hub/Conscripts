//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Fixed/Skill/RifleStimulant.h"
#include "PW.h"

URifleStimulant::URifleStimulant()
{
	skillName = NSLOCTEXT("Skill", "RifleStimulant_Name", "각성제 투여");
	skillDescription = NSLOCTEXT("Skill", "RifleStimulant_Description", "자신에게 1턴간 명중 +10, 회피 +10, 치명 +10, 주는 피해 +10%, 받는 피해 -10%를 부여한다.");

	//피해가 없는 자기 버프 스킬
	skillType = ESkillType::Buff;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::Self;
	bBowOnCast = true;
	pickTeam = EPickTeam::AllyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 0.f;
	pickCount = 1;

	battleResourceCost = 0;
	actionPointCost = 1;

	damageRatio = 0.f;
	bonusAccuracy = 0.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;
}
