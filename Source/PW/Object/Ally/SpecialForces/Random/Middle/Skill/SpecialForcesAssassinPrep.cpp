//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Skill/SpecialForcesAssassinPrep.h"
#include "PW.h"
#include "Object/Ally/SpecialForces/Random/Middle/Buff/SpecialForcesAssassinPrepBuff.h"

USpecialForcesAssassinPrep::USpecialForcesAssassinPrep()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesAssassinPrep_Name", "암살 준비");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesAssassinPrep_Description", "자신에게 2턴간 회피 +15, 치명 +10를 부여한다.");

	//피해가 없는 자기 버프 스킬
	skillType = ESkillType::Buff;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::Self;
	bBowOnCast = true;
	pickTeam = EPickTeam::AllyOnly;
	areaTarget = EAreaTarget::None;

	pickRange = 0.f;
	pickCount = 1;

	battleResourceCost = 1;
	actionPointCost = 2;

	damageRatio = 0.f;
	bonusAccuracy = 0.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	//BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	buffs.Add(USpecialForcesAssassinPrepBuff::StaticClass());
}
