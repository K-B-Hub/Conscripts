//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Fixed/RifleCoveringFire.h"
#include "PW.h"

URifleCoveringFire::URifleCoveringFire()
{
	skillName = NSLOCTEXT("Skill", "RifleCoveringFire_Name", "엄호 사격");
	skillDescription = NSLOCTEXT("Skill", "RifleCoveringFire_Description", "아군 1명을 지정해 그 주변의 적들을 사격한다");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Area;
	//지정은 아군, 실제 피해 대상은 범위 안의 적
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::AllyOnly;
	areaTarget = EAreaTarget::EnemyOnly;
	areaForm = EAreaForm::Circle;
	areaParameter1 = 2.f * CmPerMeter;

	pickRange = 8.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 1;
	actionPointCost = 2;

	//공격력 0.8~1.2배의 변동 피해
	damageRatio = 0.8f;
	damageRatioMax = 1.2f;
	bonusAccuracy = 75.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}
