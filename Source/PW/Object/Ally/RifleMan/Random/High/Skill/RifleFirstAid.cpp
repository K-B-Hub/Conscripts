//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/High/Skill/RifleFirstAid.h"
#include "Characters/CharacterBase.h"

URifleFirstAid::URifleFirstAid()
{
	skillName = NSLOCTEXT("Skill", "RifleFirstAid_Name", "응급처치");
	skillDescription = NSLOCTEXT("Skill", "RifleFirstAid_Description", "응급처치로 자신의 체력을 20% 회복한다");
	skillGrade = EUpgradeGrade::High;

	//회복 스킬, 대상은 자신뿐이라 아군 판정으로 반드시 적중한다
	skillType = ESkillType::Heal;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::Self;
	pickTeam = EPickTeam::AllyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 0.f;
	pickCount = 1;

	battleResourceCost = 0;
	actionPointCost = 1;

	//회복량은 SetCalcedStats가 최대 체력에서 직접 넣는다
	damageRatio = 0.f;
	damageRatioMax = 0.f;
	bonusAccuracy = 0.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	//플레이스홀더 몽타주, 처치 애니메이션으로 교체 예정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}

void URifleFirstAid::SetCalcedStats()
{
	Super::SetCalcedStats();

	const ACharacterBase* o = GetOwner();
	if (!o) return;

	//공격력 기반 피해량을 최대 체력 비율로 갈아끼운다, 회복 적용과 인디케이터 표기가 같은 값을 쓴다
	//피해 증폭은 회복 산식에도 곱해지므로 주는 피해가 오른 캐릭터는 회복량도 함께 오른다
	calcDamage = FMath::Max(1.f, o->GetMaxHp() * healRatio);
	calcDamageMax = calcDamage;
}
