//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/ActiveSkill/RunSkill.h"
#include "Characters/CharacterBase.h"
#include "Animation/AnimMontage.h"

URunSkill::URunSkill()
{
	skillName = NSLOCTEXT("Skill", "Run_Name", "달리기");
	skillDescription = NSLOCTEXT("Skill", "Run_Description", "행동력을 소모해 이번 턴 이동력을 추가로 얻는다");

	skillType = ESkillType::Buff;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::Self;
	pickTeam = EPickTeam::AllyOnly;
	areaTarget = EAreaTarget::None;
	areaForm = EAreaForm::Circle;

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

	//플레이스홀더 몽타주, 실제 달리기 준비 애니메이션으로 교체 예정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}

void URunSkill::BeginUse()
{
	//행동력·자원 차감과 몽타주는 기본 흐름 그대로
	Super::BeginUse();

	//이동력 추가 획득
	if (ACharacterBase* ownerPtr = GetOwner())
	{
		ownerPtr->GainMovingPoint(runBonusMovingPoint);
	}
}