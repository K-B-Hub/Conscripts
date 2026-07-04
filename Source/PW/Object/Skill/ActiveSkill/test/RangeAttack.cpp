//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/ActiveSkill/test/RangeAttack.h"

URangeAttack::URangeAttack()
{
	skillName = NSLOCTEXT("Skill", "RangeAttack_Name", "범위 원거리 공격");
	skillDescription = NSLOCTEXT("Skill", "RangeAttack_Description", "범위 내의 대상들을 공격합니다.");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Area;
	selectMode = ESelectMode::GroundPoint;
	pickTeam = EPickTeam::EnemyOnly;				//지면 대상이니 확인 안함
	areaTarget = EAreaTarget::All;
	areaForm = EAreaForm::Circle;
	areaParameter1 = 200.f;
	//areaParameter2 = 0.f;						//Circle이니 확인 안함

	pickRange = 800.f;
	pickCount = 1;
	
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

void URangeAttack::Execute(const ACharacterBase* targets)
{
	Super::Execute(targets);
}
