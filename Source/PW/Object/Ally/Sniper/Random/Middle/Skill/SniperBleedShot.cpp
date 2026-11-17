//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Middle/Skill/SniperBleedShot.h"
#include "PW.h"
#include "Object/Ally/Sniper/Random/Middle/Ailment/SniperBleedAilment.h"

USniperBleedShot::USniperBleedShot()
{
	skillName = NSLOCTEXT("Skill", "SniperBleedShot_Name", "출혈탄");
	skillDescription = NSLOCTEXT("Skill", "SniperBleedShot_Description", "20m 내의 적 1명에게 공격력 × 0.9의 원거리 피해를 주고 3턴간 턴 시작마다 최대 체력의 10% 피해를 주는 출혈을 부여한다(체력을 1 아래로 깎지 않음).\n명중 +65");
	skillGrade = EUpgradeGrade::Mid;

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 20.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 1;
	actionPointCost = 2;

	damageRatio = 0.9f;
	bonusAccuracy = 65.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	//BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	ailments.Add(USniperBleedAilment::StaticClass());

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
