//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/High/Skill/RifleStickyBomb.h"
#include "PW.h"

URifleStickyBomb::URifleStickyBomb()
{
	skillName = NSLOCTEXT("Skill", "RifleStickyBomb_Name", "점착 폭탄 투척");
	skillDescription = NSLOCTEXT("Skill", "RifleStickyBomb_Description", "적에게 점착 폭탄을 붙인다. 대상의 다음 턴 시작에 터져 주변 3m에 공격력 1.0~1.5배의 피해를 준다");
	skillGrade = EUpgradeGrade::High;

	//부착 대상은 한 명, 폭발 범위는 상태이상이 들고 있어 여기선 광역이 아니다
	skillType = ESkillType::Throw;
	damageType = EDamageType::Area;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 4.f * CmPerMeter;
	pickCount = 1;

	battleResourceCost = 0;
	actionPointCost = 2;

	//즉발 피해 없음, 피해는 폭발 시 상태이상이 준다
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
