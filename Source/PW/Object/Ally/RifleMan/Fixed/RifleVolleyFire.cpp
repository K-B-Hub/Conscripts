//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Fixed/RifleVolleyFire.h"
#include "PW.h"

URifleVolleyFire::URifleVolleyFire()
{
	skillName = NSLOCTEXT("Skill", "RifleVolleyFire_Name", "일제 사격");
	skillDescription = NSLOCTEXT("Skill", "RifleVolleyFire_Description", "전방 부채꼴 범위의 적들을 일제히 사격한다");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Area;
	//부채꼴은 시전자에 붙어 방향만 조준한다
	selectMode = ESelectMode::GroundPoint;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::All;
	areaForm = EAreaForm::Cone;
	//반지름 8m, 중심각 90도
	areaParameter1 = 8.f * CmPerMeter;
	areaParameter2 = 90.f;

	//Cone은 형태 자체가 사거리라 조준 지점 사거리를 쓰지 않는다
	pickRange = 0.f;
	pickCount = 1;

	battleResourceCost = 7;
	actionPointCost = 3;

	damageRatio = 1.1f;
	bonusAccuracy = 80.f;
	bonusCritical = 0.f;
	baseDamage = 5;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}
