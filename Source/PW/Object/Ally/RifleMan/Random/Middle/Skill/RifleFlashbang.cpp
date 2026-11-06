//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Middle/Skill/RifleFlashbang.h"
#include "PW.h"

URifleFlashbang::URifleFlashbang()
{
	skillName = NSLOCTEXT("Skill", "RifleFlashbang_Name", "섬광탄 투척");
	skillDescription = NSLOCTEXT("Skill", "RifleFlashbang_Description", "9m 내의 지점에 던져 반경 1m 안의 모든 캐릭터(아군 포함)에게 2턴간 명중 -15 · 회피 -15를 부여한다.");
	skillGrade = EUpgradeGrade::Mid;

	skillType = ESkillType::Throw;
	damageType = EDamageType::Area;
	selectMode = ESelectMode::GroundPoint;
	//아군도 휩쓸린다
	pickTeam = EPickTeam::Any;
	areaTarget = EAreaTarget::All;
	areaForm = EAreaForm::Circle;
	areaParameter1 = 1.f * CmPerMeter;

	pickRange = 9.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 0;
	actionPointCost = 1;

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