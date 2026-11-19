//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Top/Skill/SniperLastBullet.h"
#include "PW.h"

USniperLastBullet::USniperLastBullet()
{
	skillName = NSLOCTEXT("Skill", "SniperLastBullet_Name", "비장의 한 발");
	skillDescription = NSLOCTEXT("Skill", "SniperLastBullet_Description", "25m 내의 적 1명에게 공격력 × 1.6 + 8의 원거리 피해를 준다.\n명중 +90, 치명 +15");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 25.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 4;
	actionPointCost = 3;

	damageRatio = 1.6f;
	bonusAccuracy = 90.f;
	bonusCritical = 15.f;
	baseDamage = 8;
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
