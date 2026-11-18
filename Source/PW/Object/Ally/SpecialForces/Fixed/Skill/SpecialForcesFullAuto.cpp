//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Fixed/Skill/SpecialForcesFullAuto.h"
#include "PW.h"

USpecialForcesFullAuto::USpecialForcesFullAuto()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesFullAuto_Name", "전탄 사격");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesFullAuto_Description", "6m 내의 적을 4번 지정해 각각 공격력 × 0.9의 원거리 피해를 준다(같은 대상 중복 가능).\n명중 +75, 치명 +5");

	//4회 사격을 멀티픽으로 표현, 같은 대상을 거듭 고르면 그만큼 중복 적중한다
	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 6.f * CmPerMeter;
	pickCount = 4;

	battleResourceCost = 4;
	actionPointCost = 3;

	damageRatio = 0.9f;
	bonusAccuracy = 75.f;
	bonusCritical = 5.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Soldier/AM_Soldier_BurstFire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> CrouchMontageAsset(TEXT("/Game/Animation/Soldier/AM_Soldier_BurstFire_Crouch"));
	if (CrouchMontageAsset.Succeeded())
	{
		crouchSkillMontage = CrouchMontageAsset.Object;
	}
}
