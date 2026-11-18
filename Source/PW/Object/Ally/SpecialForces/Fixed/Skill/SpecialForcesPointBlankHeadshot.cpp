//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Fixed/Skill/SpecialForcesPointBlankHeadshot.h"
#include "PW.h"

USpecialForcesPointBlankHeadshot::USpecialForcesPointBlankHeadshot()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesPointBlankHeadshot_Name", "근거리 헤드샷");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesPointBlankHeadshot_Description", "3m 내의 적 1명에게 공격력 × 1.05의 원거리 피해를 준다.\n명중 +60, 치명 +15");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 3.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 1;
	actionPointCost = 2;

	damageRatio = 1.05f;
	bonusAccuracy = 60.f;
	bonusCritical = 15.f;
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
