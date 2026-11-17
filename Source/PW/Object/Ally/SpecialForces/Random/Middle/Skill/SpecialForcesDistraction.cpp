//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Skill/SpecialForcesDistraction.h"
#include "PW.h"
#include "Object/Ally/SpecialForces/Random/Middle/Buff/SpecialForcesDistractionDebuff.h"

USpecialForcesDistraction::USpecialForcesDistraction()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesDistraction_Name", "교란");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesDistraction_Description", "8m 내의 적 1명에게 2턴간 명중 -20을 부여한다.");
	skillGrade = EUpgradeGrade::Mid;

	//피해가 없는 단일 디버프 스킬, 대상 뒤쪽 땅을 쏘는 동작이라 사격 몽타주를 쓴다
	skillType = ESkillType::Buff;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 8.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 1;
	actionPointCost = 1;

	//피해 없음, 명중 굴림은 100으로 확정 처리
	damageRatio = 0.f;
	damageRatioMax = 0.f;
	bonusAccuracy = 100.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	//BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	buffs.Add(USpecialForcesDistractionDebuff::StaticClass());

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Soldier/AM_Soldier_ShotSingle"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> CrouchMontageAsset(TEXT("/Game/Animation/Soldier/AM_Soldier_ShotSingle_Crouch"));
	if (CrouchMontageAsset.Succeeded())
	{
		crouchSkillMontage = CrouchMontageAsset.Object;
	}
}
