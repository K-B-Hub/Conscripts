//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Basic/RifleBayonet.h"
#include "PW.h"

URifleBayonet::URifleBayonet()
{
	skillName = NSLOCTEXT("Skill", "RifleBayonet_Name", "총검술");
	skillDescription = NSLOCTEXT("Skill", "RifleBayonet_Description", "근접한 대상에게 공격을 가한다");

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
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	//플레이스홀더 몽타주, 총검 찌르기 애니메이션으로 교체 예정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}
