//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Fixed/Skill/RifleAimedShot.h"
#include "PW.h"

URifleAimedShot::URifleAimedShot()
{
	skillName = NSLOCTEXT("Skill", "RifleAimedShot_Name", "조준 사격");
	skillDescription = NSLOCTEXT("Skill", "RifleAimedShot_Description", "조준해 사격한다");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 12.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 1;
	actionPointCost = 2;

	damageRatio = 1.0f;
	bonusAccuracy = 70.f;
	bonusCritical = 0.f;
	baseDamage = 3;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}
