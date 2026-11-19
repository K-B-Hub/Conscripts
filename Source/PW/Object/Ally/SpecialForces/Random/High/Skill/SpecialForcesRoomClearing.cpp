//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/High/Skill/SpecialForcesRoomClearing.h"
#include "PW.h"

USpecialForcesRoomClearing::USpecialForcesRoomClearing()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesRoomClearing_Name", "실내 소탕");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesRoomClearing_Description", "전방 반지름 5m, 중심각 60°의 부채꼴 안의 모든 캐릭터(아군 포함)에게 공격력 × 1.0의 원거리 피해를 준다.\n명중 +75");

	//부채꼴은 시전자에 붙어 방향만 조준한다
	skillType = ESkillType::Ranged;
	damageType = EDamageType::Area;
	selectMode = ESelectMode::GroundPoint;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::All;
	areaForm = EAreaForm::Cone;
	areaParameter1 = 5.f * CmPerMeter;
	areaParameter2 = 60.f;

	pickRange = 0.f;
	pickCount = 1;

	battleResourceCost = 4;
	actionPointCost = 3;

	damageRatio = 1.0f;
	bonusAccuracy = 75.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Soldier/AM_Soldier_VolleyFire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> CrouchMontageAsset(TEXT("/Game/Animation/Soldier/AM_Soldier_VolleyFire_Crouch"));
	if (CrouchMontageAsset.Succeeded())
	{
		crouchSkillMontage = CrouchMontageAsset.Object;
	}
}
