//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Basic/RifleReload.h"
#include "PW.h"
#include "Characters/CharacterBase.h"

URifleReload::URifleReload()
{
	skillName = NSLOCTEXT("Skill", "RifleReload_Name", "재장전");
	skillDescription = NSLOCTEXT("Skill", "RifleReload_Description", "탄창을 보충한다");

	//피해가 없는 자기 대상 스킬
	skillType = ESkillType::Buff;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::Self;
	pickTeam = EPickTeam::AllyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 0.f;
	pickCount = 1;

	battleResourceCost = 0;
	actionPointCost = 2;

	damageRatio = 0.f;
	bonusAccuracy = 0.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	//플레이스홀더 몽타주, 재장전 애니메이션으로 교체 예정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}

void URifleReload::OnCommit()
{
	Super::OnCommit();

	//최대치 초과분은 GainBattleResource가 클램프
	if (ACharacterBase* ownerPtr = GetOwner())
	{
		ownerPtr->GainBattleResource(reloadAmount);
	}
}
