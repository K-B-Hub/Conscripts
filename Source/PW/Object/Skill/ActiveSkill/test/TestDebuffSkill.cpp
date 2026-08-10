// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/ActiveSkill/test/TestDebuffSkill.h"

#include "Object/Buff/test/TestDebuff.h"

UTestDebuffSkill::UTestDebuffSkill()
{
	skillName = NSLOCTEXT("Skill", "TestDeBuff_Name", "단일 디버프");
	skillDescription = NSLOCTEXT("Skill", "TestDeBuff_Description", "단일 대상에게 디버프 적용");

	skillType = ESkillType::Buff;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;
	areaForm = EAreaForm::Circle;
	areaParameter1 = 50.f;
	//areaParameter2 = 0.f;						//Circle이니 확인 안함

	pickRange = 800.f;
	pickCount = 1;
	
	battleResourceCost = 0;
	actionPointCost = 2;
	
	damageRatio = 0.f;
	bonusAccuracy = 20.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
	
	buffs.Add(UTestDebuff::StaticClass());
}

void UTestDebuffSkill::Execute(const ACharacterBase* target)
{
	Super::Execute(target);
}
