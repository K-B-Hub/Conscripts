//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Fixed/Skill/SniperAimedSnipe.h"
#include "PW.h"

USniperAimedSnipe::USniperAimedSnipe()
{
	skillName = NSLOCTEXT("Skill", "SniperAimedSnipe_Name", "조준 저격");
	skillDescription = NSLOCTEXT("Skill", "SniperAimedSnipe_Description", "20m 내의 적 1명에게 공격력 × 1.05 + 2의 원거리 피해를 준다.\n명중 +70");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 20.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 1;
	actionPointCost = 2;

	damageRatio = 1.05f;
	bonusAccuracy = 70.f;
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
