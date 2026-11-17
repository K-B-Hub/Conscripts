//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/High/Skill/SpecialForcesFlashBreach.h"
#include "PW.h"
#include "Object/Ally/SpecialForces/Random/High/Buff/SpecialForcesFlashBreachDebuff.h"

USpecialForcesFlashBreach::USpecialForcesFlashBreach()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesFlashBreach_Name", "섬광 돌입");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesFlashBreach_Description", "4m 내의 지점에 던져 반경 2m 안의 적 전원에게 1턴간 명중 -20, 회피 -20을 부여한다.");
	skillGrade = EUpgradeGrade::High;

	skillType = ESkillType::Throw;
	damageType = EDamageType::Area;
	selectMode = ESelectMode::GroundPoint;
	pickTeam = EPickTeam::Any;
	areaTarget = EAreaTarget::EnemyOnly;
	areaForm = EAreaForm::Circle;
	areaParameter1 = 2.f * CmPerMeter;

	pickRange = 4.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 1;
	actionPointCost = 2;

	//피해 없음, 명중 굴림은 100으로 확정 처리
	damageRatio = 0.f;
	damageRatioMax = 0.f;
	bonusAccuracy = 100.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	//궤적만 표시하고 시전자는 이동하지 않는다
	arcApexRatio = 0.5f;

	//BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	buffs.Add(USpecialForcesFlashBreachDebuff::StaticClass());

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Soldier/AM_Soldier_Throw"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> CrouchMontageAsset(TEXT("/Game/Animation/Soldier/AM_Soldier_Throw_Crouch"));
	if (CrouchMontageAsset.Succeeded())
	{
		crouchSkillMontage = CrouchMontageAsset.Object;
	}
}
