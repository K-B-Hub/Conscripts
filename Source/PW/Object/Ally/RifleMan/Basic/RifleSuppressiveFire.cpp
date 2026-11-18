//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Basic/RifleSuppressiveFire.h"
#include "PW.h"

URifleSuppressiveFire::URifleSuppressiveFire()
{
	skillName = NSLOCTEXT("Skill", "RifleSuppressiveFire_Name", "견제 사격");
	skillDescription = NSLOCTEXT("Skill", "RifleSuppressiveFire_Description", "24m 내의 적 1명에게 공격력 × 1.0 + 3의 원거리 피해를 준다.\n명중 +70");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 24.f * CmPerMeter;
	pickCount = 1;

	//기본 공격은 탄창을 쓰지 않는다
	battleResourceCost = 0;
	actionPointCost = 2;

	damageRatio = 1.0f;
	bonusAccuracy = 70.f;
	bonusCritical = 0.f;
	baseDamage = 3;
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
