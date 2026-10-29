//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Fixed/Skill/RiflePreciseShot.h"
#include "PW.h"

URiflePreciseShot::URiflePreciseShot()
{
	skillName = NSLOCTEXT("Skill", "RiflePreciseShot_Name", "정밀 사격");
	skillDescription = NSLOCTEXT("Skill", "RiflePreciseShot_Description", "정밀하게 조준해 치명적인 일격을 노린다");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 28.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 2;
	actionPointCost = 2;

	damageRatio = 1.2f;
	bonusAccuracy = 80.f;
	bonusCritical = 20.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}
