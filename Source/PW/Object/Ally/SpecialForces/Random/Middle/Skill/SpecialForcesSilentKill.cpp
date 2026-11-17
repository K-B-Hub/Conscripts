//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Skill/SpecialForcesSilentKill.h"
#include "PW.h"

USpecialForcesSilentKill::USpecialForcesSilentKill()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesSilentKill_Name", "소리 없는 처리");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesSilentKill_Description", "1m 내의 적 1명에게 공격력 × 1.0의 근접 피해를 준다.\n명중 +85, 치명 +30");
	skillGrade = EUpgradeGrade::Mid;

	skillType = ESkillType::Melee;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 1.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 0;
	actionPointCost = 2;

	damageRatio = 1.0f;
	bonusAccuracy = 85.f;
	bonusCritical = 30.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Soldier/AM_Soldier_MeleeLight"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}
