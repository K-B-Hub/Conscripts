//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Middle/Skill/SniperSteadyAim.h"
#include "Characters/CharacterBase.h"

USniperSteadyAim::USniperSteadyAim()
{
	skillName = NSLOCTEXT("Skill", "SniperSteadyAim_Name", "정조준");
	skillDescription = NSLOCTEXT("Skill", "SniperSteadyAim_Description", "이번 턴에 이동하지 않고 공격 시 명중 +10, 치명 +5");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	accuracy = 10.f;
	critical = 5.f;
}

bool USniperSteadyAim::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	if (!target) return false;
	ACharacterBase* o = owner.Get();
	if (!o) return false;

	//아군 대상 스킬은 제외
	if (o->IsAlly() == target->IsAlly()) return false;

	//isMoved는 인디케이터가 투기적으로 켜므로 실제 이동력 소모로 판정
	return !o->HasConsumedMovingPoint();
}
