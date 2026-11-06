//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/High/Skill/RifleFirebomb.h"
#include "PW.h"
#include "Actors/Terrain/FireTerrain.h"

URifleFirebomb::URifleFirebomb()
{
	skillName = NSLOCTEXT("Skill", "RifleFirebomb_Name", "화염병 투척");
	skillDescription = NSLOCTEXT("Skill", "RifleFirebomb_Description", "9m 내의 지점에 던져 반경 1m를 3라운드간 화염 지형으로 만든다. 화염 안에서 턴을 시작하거나 끝낼 때마다 2의 피해를 입는다(체력을 1 아래로 깎지 않음).");
	skillGrade = EUpgradeGrade::High;

	skillType = ESkillType::Throw;
	damageType = EDamageType::Area;
	selectMode = ESelectMode::GroundPoint;
	//아군도 휩쓸린다
	pickTeam = EPickTeam::Any;
	//피해가 없어도 범위를 데칼로 보여주기 위해 광역으로 둔다
	areaTarget = EAreaTarget::All;
	areaForm = EAreaForm::Circle;
	areaParameter1 = 1.f * CmPerMeter;

	pickRange = 9.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 0;
	actionPointCost = 2;

	//즉발 피해 없음, 피해는 지형이 턴마다 준다
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
	spawnTerrainClass = AFireTerrain::StaticClass();

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
