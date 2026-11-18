//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Top/Skill/SpecialForcesSilentHeadshot.h"
#include "PW.h"
#include "Object/Ally/SpecialForces/Random/Top/Buff/SpecialForcesSilentHeadshotDebuff.h"

USpecialForcesSilentHeadshot::USpecialForcesSilentHeadshot()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesSilentHeadshot_Name", "무음 헤드샷");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesSilentHeadshot_Description", "4m 내의 적 1명에게 공격력 × 1.2의 원거리 피해를 주고 1턴간 행동력 -1을 부여한다.\n명중 +90, 치명 +25");
	skillGrade = EUpgradeGrade::Top;

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 4.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 1;
	actionPointCost = 2;

	damageRatio = 1.2f;
	bonusAccuracy = 90.f;
	bonusCritical = 25.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	//BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	buffs.Add(USpecialForcesSilentHeadshotDebuff::StaticClass());

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
