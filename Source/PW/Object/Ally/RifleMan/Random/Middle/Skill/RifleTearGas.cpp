//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Middle/Skill/RifleTearGas.h"
#include "PW.h"

URifleTearGas::URifleTearGas()
{
	skillName = NSLOCTEXT("Skill", "RifleTearGas_Name", "최루탄 투척");
	skillDescription = NSLOCTEXT("Skill", "RifleTearGas_Description", "최루탄을 던져 착지 지점 1m 내 캐릭터에게 2턴간 최대 체력 10%의 피해를 준다");
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
	actionPointCost = 2;

	//즉발 피해 없음, 피해는 상태이상이 턴마다 준다
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