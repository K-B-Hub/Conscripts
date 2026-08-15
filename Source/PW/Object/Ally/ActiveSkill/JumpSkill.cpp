//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Ally/ActiveSkill/JumpSkill.h"
#include "Characters/CharacterBase.h"
#include "Animation/AnimMontage.h"

UJumpSkill::UJumpSkill()
{
	skillName = NSLOCTEXT("Skill", "Jump_Name", "점프");
	skillDescription = NSLOCTEXT("Skill", "Jump_Description", "포물선으로 도약해 낮은 장애물과 지형을 넘는다. 도약 거리의 1.5배만큼 이동력을 소모한다");

	//다른 Throw들과 구별 위해 Buff로 표기
	skillType = ESkillType::Buff;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::GroundPoint;
	pickTeam = EPickTeam::AllyOnly;
	areaTarget = EAreaTarget::None;
	areaForm = EAreaForm::Circle;
	areaParameter1 = 100.f;

	//기본 이동력(9m) 기준 도약 사거리(cm), 상한이 아니라 표시용 기준값
	//실제 사거리는 항상 현재 이동력이 결정 (GetEffectivePickRange)
	pickRange = 600.f;
	pickCount = 1;

	battleResourceCost = 0;
	//점프는 이동력만 소모, 행동력은 사용 안 함
	actionPointCost = 0;

	damageRatio = 0.f;
	bonusAccuracy = 0.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	//거리에 비례해 정점이 높아지는 포물선, 이동력 소모 배율
	arcApexRatio = 0.5f;
	arcMoveCostMultiplier = 1.5f;

	//플레이스홀더 몽타주, 실제  점프 준비 애니메이션으로 교체 예정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_JumpUp"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}

bool UJumpSkill::CanExecute() const
{
	//포복 자세에서는 도약 불가
	ACharacterBase* ownerPtr = GetOwner();
	if (ownerPtr && ownerPtr->IsProne()) return false;

	return Super::CanExecute();
}