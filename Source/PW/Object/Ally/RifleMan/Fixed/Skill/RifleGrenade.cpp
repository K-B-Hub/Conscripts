//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Fixed/Skill/RifleGrenade.h"
#include "PW.h"

URifleGrenade::URifleGrenade()
{
	skillName = NSLOCTEXT("Skill", "RifleGrenade_Name", "수류탄 투척");
	skillDescription = NSLOCTEXT("Skill", "RifleGrenade_Description", "수류탄을 던져 착지 지점 범위에 피해를 준다");

	skillType = ESkillType::Throw;
	damageType = EDamageType::Area;
	selectMode = ESelectMode::GroundPoint;
	pickTeam = EPickTeam::Any;
	areaTarget = EAreaTarget::All;
	areaForm = EAreaForm::Circle;
	areaParameter1 = 2.f * CmPerMeter;

	pickRange = 6.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 0;
	actionPointCost = 2;

	//공격력 0.7~1.2배의 변동 피해
	damageRatio = 0.7f;
	damageRatioMax = 1.2f;
	bonusAccuracy = 70.f;
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
