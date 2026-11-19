//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Low/Skill/SpecialForcesBreachShot.h"
#include "PW.h"

USpecialForcesBreachShot::USpecialForcesBreachShot()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesBreachShot_Name", "돌입 사격");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesBreachShot_Description", "6m 내의 적 1명에게 공격력 × 1.0 + 2의 원거리 피해를 준다.\n명중 +85");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 6.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 1;
	actionPointCost = 2;

	damageRatio = 1.0f;
	bonusAccuracy = 85.f;
	bonusCritical = 0.f;
	baseDamage = 2;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Soldier/AM_Soldier_ShotSingle"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> CrouchMontageAsset(TEXT("/Game/Animation/Soldier/AM_Soldier_ShotSingle_Crouch"));
	if (CrouchMontageAsset.Succeeded())
	{
		crouchSkillMontage = CrouchMontageAsset.Object;
	}
}
