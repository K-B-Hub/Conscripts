//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Low/Skill/SniperDisarmShot.h"
#include "PW.h"
#include "Object/Ally/Sniper/Random/Low/Buff/SniperDisarmDebuff.h"

USniperDisarmShot::USniperDisarmShot()
{
	skillName = NSLOCTEXT("Skill", "SniperDisarmShot_Name", "무력화 사격");
	skillDescription = NSLOCTEXT("Skill", "SniperDisarmShot_Description", "18m 내의 적 1명에게 공격력 × 0.7 + 2의 원거리 피해를 주고 1턴간 명중 -30을 부여한다.\n명중 +70");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 18.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 1;
	actionPointCost = 2;

	damageRatio = 0.7f;
	bonusAccuracy = 70.f;
	bonusCritical = 0.f;
	baseDamage = 2;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	//BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	buffs.Add(USniperDisarmDebuff::StaticClass());

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
