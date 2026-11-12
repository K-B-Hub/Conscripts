//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Fixed/Skill/RiflePreciseShot.h"
#include "PW.h"

URiflePreciseShot::URiflePreciseShot()
{
	skillName = NSLOCTEXT("Skill", "RiflePreciseShot_Name", "정밀 사격");
	skillDescription = NSLOCTEXT("Skill", "RiflePreciseShot_Description", "28m 내의 적 1명에게 공격력 × 1.2의 원거리 피해를 준다.\n명중 +80, 치명 +20");

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
