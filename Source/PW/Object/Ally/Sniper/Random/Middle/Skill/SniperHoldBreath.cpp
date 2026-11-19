//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Middle/Skill/SniperHoldBreath.h"
#include "PW.h"
#include "Object/Ally/Sniper/Random/Middle/Buff/SniperHoldBreathBuff.h"

USniperHoldBreath::USniperHoldBreath()
{
	skillName = NSLOCTEXT("Skill", "SniperHoldBreath_Name", "숨 참기");
	skillDescription = NSLOCTEXT("Skill", "SniperHoldBreath_Description", "자신에게 2턴간 명중 +15, 치명 +10, 주는 피해 +10%를 부여한다.");

	//피해가 없는 자기 버프 스킬
	skillType = ESkillType::Buff;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::Self;
	bBowOnCast = true;
	pickTeam = EPickTeam::AllyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 0.f;
	pickCount = 1;

	battleResourceCost = 0;
	actionPointCost = 2;

	damageRatio = 0.f;
	bonusAccuracy = 0.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	//BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	buffs.Add(USniperHoldBreathBuff::StaticClass());
}
