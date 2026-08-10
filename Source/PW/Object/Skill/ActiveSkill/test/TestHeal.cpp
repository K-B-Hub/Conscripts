// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/ActiveSkill/test/TestHeal.h"

UTestHeal::UTestHeal()
{
	skillName = NSLOCTEXT("Skill", "TestHeal_Name", "단일 회복");
	skillDescription = NSLOCTEXT("Skill", "TestHeal_Description", "단일 대상에게 회복 적용");

	skillType = ESkillType::Heal;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::AllyOnly;
	areaTarget = EAreaTarget::None;
	areaForm = EAreaForm::Circle;
	areaParameter1 = 50.f;
	//areaParameter2 = 0.f;						//Circle이니 확인 안함

	pickRange = 1200.f;
	pickCount = 1;
	
	battleResourceCost = 0;
	actionPointCost = 2;
	
	damageRatio = 1.0f;
	bonusAccuracy = 35.f;
	bonusCritical = 0.f;
	baseDamage = 1;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}

void UTestHeal::Execute(const ACharacterBase* target)
{
	Super::Execute(target);
}
