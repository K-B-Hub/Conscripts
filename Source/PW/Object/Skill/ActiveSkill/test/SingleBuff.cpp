// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/ActiveSkill/test/SingleBuff.h"
#include "Object/Buff/test/TestBuff.h"

USingleBuff::USingleBuff()
{
	skillName = NSLOCTEXT("Skill", "SingleBuff_Name", "단일 버프");
	skillDescription = NSLOCTEXT("Skill", "SingleBuff_Description", "단일 대상에게 버프 적용");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::Any;
	areaTarget = EAreaTarget::None;
	areaForm = EAreaForm::Circle;
	areaParameter1 = 50.f;
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
	
	buffs.Add(UTestBuff::StaticClass());
}

void USingleBuff::Execute(const TArray<ACharacterBase*>& targets)
{
	Super::Execute(targets);
}
