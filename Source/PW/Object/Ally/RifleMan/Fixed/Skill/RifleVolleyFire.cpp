//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Fixed/Skill/RifleVolleyFire.h"
#include "PW.h"

URifleVolleyFire::URifleVolleyFire()
{
	skillName = NSLOCTEXT("Skill", "RifleVolleyFire_Name", "일제 사격");
	skillDescription = NSLOCTEXT("Skill", "RifleVolleyFire_Description", "전방 반지름 8m, 중심각 90°의 부채꼴 안의 모든 캐릭터(아군 포함)에게 공격력 × 1.1 + 5의 원거리 피해를 준다.\n명중 +80");

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
