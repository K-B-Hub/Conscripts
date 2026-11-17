//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Middle/Skill/SniperHarassShot.h"
#include "PW.h"

USniperHarassShot::USniperHarassShot()
{
	skillName = NSLOCTEXT("Skill", "SniperHarassShot_Name", "견제 저격");
	skillDescription = NSLOCTEXT("Skill", "SniperHarassShot_Description", "18m 내의 적 1명에게 공격력 × 0.6의 원거리 피해를 준다.\n명중 +50");
	skillGrade = EUpgradeGrade::Mid;

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 18.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 1;
	actionPointCost = 1;

	damageRatio = 0.6f;
	bonusAccuracy = 50.f;
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
