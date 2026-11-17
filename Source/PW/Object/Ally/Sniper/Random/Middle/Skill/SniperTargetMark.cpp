//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Middle/Skill/SniperTargetMark.h"
#include "PW.h"
#include "Object/Ally/Sniper/Random/Middle/Buff/SniperTargetMarkDebuff.h"

USniperTargetMark::USniperTargetMark()
{
	skillName = NSLOCTEXT("Skill", "SniperTargetMark_Name", "조준 표식");
	skillDescription = NSLOCTEXT("Skill", "SniperTargetMark_Description", "20m 내의 적 1명에게 2턴간 회피 -15를 부여한다.");
	skillGrade = EUpgradeGrade::Mid;

	//피해가 없는 단일 디버프 스킬
	skillType = ESkillType::Buff;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 20.f * CmPerMeter;
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

	//BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	buffs.Add(USniperTargetMarkDebuff::StaticClass());
}
