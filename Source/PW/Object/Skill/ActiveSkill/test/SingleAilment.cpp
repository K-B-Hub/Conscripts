//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/ActiveSkill/test/SingleAilment.h"
#include "Object/Ailment/TestAilment.h"

USingleAilment::USingleAilment()
{
	skillName = NSLOCTEXT("Skill", "SingleAilment_Name", "단일 상태이상");
	skillDescription = NSLOCTEXT("Skill", "SingleAilment_Description", "단일 대상에게 상태이상 적용");

	skillType = ESkillType::Ailment;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;
	areaForm = EAreaForm::Circle;
	areaParameter1 = 500.f;

	pickRange = 2000.f;
	pickCount = 1;

	battleResourceCost = 0;
	actionPointCost = 1;

	damageRatio = 0.f;
	bonusAccuracy = 80.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}

	ailments.Add(UTestAilment::StaticClass());
}

void USingleAilment::Execute(const ACharacterBase* target)
{
	Super::Execute(target);
}
