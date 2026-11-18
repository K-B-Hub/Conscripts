//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Basic/SpecialForcesCarbineShot.h"
#include "PW.h"

USpecialForcesCarbineShot::USpecialForcesCarbineShot()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesCarbineShot_Name", "카빈 사격");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesCarbineShot_Description", "8m 내의 적 1명에게 공격력 × 1.0의 원거리 피해를 준다.\n명중 +55");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 8.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 1;
	actionPointCost = 2;

	damageRatio = 1.0f;
	bonusAccuracy = 55.f;
	bonusCritical = 0.f;
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
