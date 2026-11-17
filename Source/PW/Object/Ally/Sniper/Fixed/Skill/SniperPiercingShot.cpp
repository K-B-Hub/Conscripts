//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Fixed/Skill/SniperPiercingShot.h"
#include "PW.h"

USniperPiercingShot::USniperPiercingShot()
{
	skillName = NSLOCTEXT("Skill", "SniperPiercingShot_Name", "관통 저격");
	skillDescription = NSLOCTEXT("Skill", "SniperPiercingShot_Description", "전방 폭 0.5m, 길이 22m의 직선 안의 모든 캐릭터(아군 포함)에게 공격력 × 1.2의 원거리 피해를 준다.\n명중 +70");

	//직선은 시전자에 붙어 방향만 조준한다
	skillType = ESkillType::Ranged;
	damageType = EDamageType::Area;
	selectMode = ESelectMode::GroundPoint;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::All;
	areaForm = EAreaForm::Ray;
	areaParameter1 = 0.5f * CmPerMeter;
	areaParameter2 = 22.f * CmPerMeter;

	pickRange = 0.f;
	pickCount = 1;

	battleResourceCost = 3;
	actionPointCost = 3;

	damageRatio = 1.2f;
	bonusAccuracy = 70.f;
	bonusCritical = 0.f;
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
