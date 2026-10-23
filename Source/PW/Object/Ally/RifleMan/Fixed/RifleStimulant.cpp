//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Fixed/RifleStimulant.h"
#include "PW.h"

URifleStimulant::URifleStimulant()
{
	skillName = NSLOCTEXT("Skill", "RifleStimulant_Name", "각성제 투여");
	skillDescription = NSLOCTEXT("Skill", "RifleStimulant_Description", "각성제를 투여해 1턴간 전투 능력이 오른다");

	//피해가 없는 자기 버프 스킬
	skillType = ESkillType::Buff;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::Self;
	pickTeam = EPickTeam::AllyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 0.f;
	pickCount = 1;

	battleResourceCost = 0;
	actionPointCost = 1;

	damageRatio = 0.f;
	bonusAccuracy = 0.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	//플레이스홀더 몽타주, 투여 애니메이션으로 교체 예정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}
