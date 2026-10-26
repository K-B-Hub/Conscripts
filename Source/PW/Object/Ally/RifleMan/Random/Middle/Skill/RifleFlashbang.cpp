//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Middle/Skill/RifleFlashbang.h"
#include "PW.h"

URifleFlashbang::URifleFlashbang()
{
	skillName = NSLOCTEXT("Skill", "RifleFlashbang_Name", "섬광탄 투척");
	skillDescription = NSLOCTEXT("Skill", "RifleFlashbang_Description", "섬광탄을 던져 착지 지점 1m 내 캐릭터의 명중과 회피를 2턴간 15 감소시킨다");
	skillGrade = EUpgradeGrade::Mid;

	skillType = ESkillType::Throw;
	damageType = EDamageType::Area;
	selectMode = ESelectMode::GroundPoint;
	//아군도 휩쓸린다
	pickTeam = EPickTeam::Any;
	areaTarget = EAreaTarget::All;
	areaForm = EAreaForm::Circle;
	areaParameter1 = 1.f * CmPerMeter;

	pickRange = 6.f * CmPerMeter;
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

	//플레이스홀더 몽타주, 투척 애니메이션으로 교체 예정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}