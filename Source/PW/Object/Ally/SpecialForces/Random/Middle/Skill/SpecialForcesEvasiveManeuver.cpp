//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Skill/SpecialForcesEvasiveManeuver.h"
#include "Characters/CharacterBase.h"

USpecialForcesEvasiveManeuver::USpecialForcesEvasiveManeuver()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesEvasiveManeuver_Name", "기동 회피");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesEvasiveManeuver_Description", "이번 턴에 이동했다면 다음 턴까지 회피 +10");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::MoveComplete;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	evasion = 10.f;
}

void USpecialForcesEvasiveManeuver::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	//isMoved는 인디케이터가 투기적으로 켜므로 실제 이동력 소모로 판정
	const bool bShouldApply = o->HasConsumedMovingPoint();
	if (bShouldApply == bApplied) return;

	bApplied = bShouldApply;
	o->ApplyPassiveStatDelta(this, bApplied ? 1 : -1);
}
