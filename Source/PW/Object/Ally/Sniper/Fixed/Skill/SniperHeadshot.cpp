//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Fixed/Skill/SniperHeadshot.h"
#include "PW.h"

USniperHeadshot::USniperHeadshot()
{
	skillName = NSLOCTEXT("Skill", "SniperHeadshot_Name", "헤드샷");
	skillDescription = NSLOCTEXT("Skill", "SniperHeadshot_Description", "22m 내의 적 1명에게 공격력 × 1.35의 원거리 피해를 준다.\n명중 +70, 치명 +20");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 22.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 2;
	actionPointCost = 3;

	damageRatio = 1.35f;
	bonusAccuracy = 70.f;
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
