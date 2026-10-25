//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Middle/RifleMorale.h"
#include "PW.h"

URifleMorale::URifleMorale()
{
	skillName = NSLOCTEXT("Skill", "RifleMorale_Name", "사기 진작");
	skillDescription = NSLOCTEXT("Skill", "RifleMorale_Description", "주변 5m 내의 아군과 자신의 명중을 2턴간 10 올린다");
	skillGrade = EUpgradeGrade::Mid;

	//피해가 없는 범위 버프, 시전자 자신도 대상에 포함된다
	skillType = ESkillType::Buff;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::Self;
	pickTeam = EPickTeam::AllyOnly;
	areaTarget = EAreaTarget::AllyOnly;
	areaForm = EAreaForm::Circle;
	areaParameter1 = 5.f * CmPerMeter;

	pickRange = 0.f;
	pickCount = 1;

	battleResourceCost = 0;
	actionPointCost = 2;

	//피해 없음, 명중 굴림은 100으로 확정 처리
	damageRatio = 0.f;
	damageRatioMax = 0.f;
	bonusAccuracy = 100.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	//플레이스홀더 몽타주, 지휘 애니메이션으로 교체 예정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}