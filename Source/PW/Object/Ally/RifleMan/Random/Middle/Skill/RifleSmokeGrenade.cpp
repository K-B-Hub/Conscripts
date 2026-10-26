//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Middle/Skill/RifleSmokeGrenade.h"
#include "PW.h"
#include "Actors/Terrain/SmokeTerrain.h"

URifleSmokeGrenade::URifleSmokeGrenade()
{
	skillName = NSLOCTEXT("Skill", "RifleSmokeGrenade_Name", "연막탄 투척");
	skillDescription = NSLOCTEXT("Skill", "RifleSmokeGrenade_Description", "연막탄을 던져 착지 지점 1m를 3라운드간 연막으로 만든다");
	skillGrade = EUpgradeGrade::Mid;

	skillType = ESkillType::Throw;
	damageType = EDamageType::Area;
	selectMode = ESelectMode::GroundPoint;
	pickTeam = EPickTeam::Any;
	//피해가 없어도 범위를 데칼로 보여주기 위해 광역으로 둔다
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

	//지속 라운드는 지형 자신의 lifeRound가 들고 있다
	spawnTerrainClass = ASmokeTerrain::StaticClass();

	//플레이스홀더 몽타주, 투척 애니메이션으로 교체 예정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}