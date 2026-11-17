//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Fixed/Skill/SniperDoubleSnipe.h"
#include "PW.h"

USniperDoubleSnipe::USniperDoubleSnipe()
{
	skillName = NSLOCTEXT("Skill", "SniperDoubleSnipe_Name", "연속 저격");
	skillDescription = NSLOCTEXT("Skill", "SniperDoubleSnipe_Description", "18m 내의 적을 2번 지정해 각각 공격력 × 0.9의 원거리 피해를 준다(같은 대상 중복 가능).\n명중 +55");

	//2회 사격을 멀티픽으로 표현, 같은 대상을 거듭 고르면 그만큼 중복 적중한다
	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 18.f * CmPerMeter;
	pickCount = 2;

	battleResourceCost = 2;
	actionPointCost = 2;

	damageRatio = 0.9f;
	bonusAccuracy = 55.f;
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
