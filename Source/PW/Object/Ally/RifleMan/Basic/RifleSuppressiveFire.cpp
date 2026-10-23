//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Basic/RifleSuppressiveFire.h"
#include "PW.h"

URifleSuppressiveFire::URifleSuppressiveFire()
{
	skillName = NSLOCTEXT("Skill", "RifleSuppressiveFire_Name", "견제 사격");
	skillDescription = NSLOCTEXT("Skill", "RifleSuppressiveFire_Description", "1명의 대상에게 총을 쏴 사격을 한다");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 12.f * CmPerMeter;
	pickCount = 1;

	//탄창 1개 소모
	battleResourceCost = 1;
	actionPointCost = 2;

	damageRatio = 1.0f;
	bonusAccuracy = 60.f;
	bonusCritical = 0.f;
	baseDamage = 1;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}
