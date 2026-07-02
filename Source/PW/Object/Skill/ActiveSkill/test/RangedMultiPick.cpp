//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/ActiveSkill/test/RangedMultiPick.h"

URangedMultiPick::URangedMultiPick()
{
	skillName = NSLOCTEXT("Skill", "SingleRangeAttack_Name", "다중 범위 공격");
	skillDescription = NSLOCTEXT("Skill", "SingleRangeAttack_Description", "2명의 대상에게 사거리 내에서 공격합니다.");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::Any;
	areaTarget = EAreaTarget::All;
	areaForm = EAreaForm::Circle;		
	areaParameter1 = 100.f;			
	areaParameter2 = 0.f;			

	pickRange = 1200.f;
	pickCount = 2;
	
	battleResourceCost = 0;
	actionPointCost = 2;
	
	damageRatio = 1.0f;
	bonusAccuracy = 35.f;
	bonusCritical = 0.f;
	baseDamage = 1;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire_Montage"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}

void URangedMultiPick::Execute(const ACharacterBase* targets)
{
	Super::Execute(targets);
}
