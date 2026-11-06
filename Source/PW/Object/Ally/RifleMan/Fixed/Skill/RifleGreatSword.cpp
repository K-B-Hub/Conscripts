//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Fixed/Skill/RifleGreatSword.h"
#include "PW.h"

URifleGreatSword::URifleGreatSword()
{
	skillName = NSLOCTEXT("Skill", "RifleGreatSword_Name", "대검술");
	skillDescription = NSLOCTEXT("Skill", "RifleGreatSword_Description", "1m 내의 적 1명에게 공격력 × 1.0 + 5의 근접 피해를 준다.\n명중 +90");

	skillType = ESkillType::Melee;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 1.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 0;
	actionPointCost = 2;

	damageRatio = 1.0f;
	bonusAccuracy = 90.f;
	bonusCritical = 0.f;
	baseDamage = 5;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Soldier/AM_Soldier_MeleeHard"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}
