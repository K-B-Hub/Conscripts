//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Fixed/Skill/RifleBurstFire.h"
#include "PW.h"

URifleBurstFire::URifleBurstFire()
{
	skillName = NSLOCTEXT("Skill", "RifleBurstFire_Name", "연사");
	skillDescription = NSLOCTEXT("Skill", "RifleBurstFire_Description", "16m 내의 적을 3번 지정해 각각 공격력 × 1.0의 원거리 피해를 준다(같은 대상 중복 가능).\n명중 +75");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 16.f * CmPerMeter;
	//3회 사격을 멀티픽으로 표현, 같은 대상을 거듭 고르면 그만큼 중복 적중한다
	pickCount = 3;

	battleResourceCost = 3;
	actionPointCost = 2;

	damageRatio = 1.0f;
	bonusAccuracy = 75.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Soldier/AM_Soldier_ShotAuto"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> CrouchMontageAsset(TEXT("/Game/Animation/Soldier/AM_Soldier_ShotAuto_Crouch"));
	if (CrouchMontageAsset.Succeeded())
	{
		crouchSkillMontage = CrouchMontageAsset.Object;
	}
}
