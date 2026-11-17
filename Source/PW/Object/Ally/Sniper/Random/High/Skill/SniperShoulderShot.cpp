//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/High/Skill/SniperShoulderShot.h"
#include "PW.h"
#include "Object/Ally/Sniper/Random/High/Buff/SniperShoulderShotDebuff.h"

USniperShoulderShot::USniperShoulderShot()
{
	skillName = NSLOCTEXT("Skill", "SniperShoulderShot_Name", "어깨 사격");
	skillDescription = NSLOCTEXT("Skill", "SniperShoulderShot_Description", "16m 내의 적 1명에게 공격력 × 0.5의 원거리 피해를 주고 1턴간 명중 -30, 치명 -40을 부여한다.\n명중 +55");
	skillGrade = EUpgradeGrade::High;

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 16.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 1;
	actionPointCost = 2;

	damageRatio = 0.5f;
	bonusAccuracy = 55.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	//BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	buffs.Add(USniperShoulderShotDebuff::StaticClass());

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
