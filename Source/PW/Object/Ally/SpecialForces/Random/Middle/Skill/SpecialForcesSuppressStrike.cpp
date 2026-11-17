//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Skill/SpecialForcesSuppressStrike.h"
#include "PW.h"

USpecialForcesSuppressStrike::USpecialForcesSuppressStrike()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesSuppressStrike_Name", "제압 타격");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesSuppressStrike_Description", "1m 내의 적 1명에게 공격력 × 0.5의 근접 피해를 주고 3m 밀쳐낸다.\n명중 +80");
	skillGrade = EUpgradeGrade::Mid;

	skillType = ESkillType::Melee;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 1.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 0;
	actionPointCost = 1;

	damageRatio = 0.5f;
	bonusAccuracy = 80.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	//명중 시 시전자 반대 방향으로 밀쳐낸다
	knockbackForce = 3.f * CmPerMeter;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Soldier/AM_Soldier_MeleeLight"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}
