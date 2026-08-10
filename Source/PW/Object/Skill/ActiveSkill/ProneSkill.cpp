//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/ActiveSkill/ProneSkill.h"
#include "Characters/CharacterBase.h"
#include "Animation/AnimMontage.h"

UProneSkill::UProneSkill()
{
	skillName = NSLOCTEXT("Skill", "Prone_Name", "포복");
	skillDescription = NSLOCTEXT("Skill", "Prone_Description", "엎드리거나 일어선다. 포복 중에는 시야가 낮아져 낮은 엄폐물에 숨을 수 있으나 이동력 소모가 2배가 된다");

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
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_ProneToStand"));
	if (MontageAsset.Succeeded())
	{
		standUpMontage = MontageAsset.Object;
	}
	
	//플레이스홀더 몽타주, 실제 달리기 준비 애니메이션으로 교체 예정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset2(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_StandToProne"));
	if (MontageAsset2.Succeeded())
	{
		lieDownMontage = MontageAsset2.Object;
	}
}

bool UProneSkill::CanExecute() const
{
	ACharacterBase* ownerPtr = GetOwner();
	if (!ownerPtr) return false;

	//포복 중이면 일어서기(무료)라 항상 가능, 서있으면 기본 행동력·자원 검사
	if (ownerPtr->IsProne()) return true;
	return Super::CanExecute();
}

void UProneSkill::BeginUse()
{
	ACharacterBase* ownerPtr = GetOwner();
	if (!ownerPtr) return;

	//서있다가 포복으로 진입할 때만 비용 소모, 일어서기는 무료
	if (!ownerPtr->IsProne())
	{
		Super::BeginUse();
	}
}

UAnimMontage* UProneSkill::GetCommitMontage() const
{
	ACharacterBase* ownerPtr = GetOwner();
	if (!ownerPtr) return nullptr;

	//토글 이전 상태가 전환 방향을 결정
	return ownerPtr->IsProne() ? standUpMontage : lieDownMontage;
}

void UProneSkill::OnCommit()
{
	if (ACharacterBase* ownerPtr = GetOwner())
	{
		ownerPtr->ToggleProne();
	}
}