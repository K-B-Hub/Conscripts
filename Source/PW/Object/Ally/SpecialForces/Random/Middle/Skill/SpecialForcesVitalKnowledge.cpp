//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Skill/SpecialForcesVitalKnowledge.h"
#include "Characters/CharacterBase.h"

USpecialForcesVitalKnowledge::USpecialForcesVitalKnowledge()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesVitalKnowledge_Name", "급소 숙지");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesVitalKnowledge_Description", "체력이 가득 찬 적 공격 시 치명 +25");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	critical = 25.f;
}

bool USpecialForcesVitalKnowledge::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	if (!target) return false;
	ACharacterBase* o = owner.Get();
	if (!o) return false;

	//아군 대상 스킬은 제외
	if (o->IsAlly() == target->IsAlly()) return false;

	return target->GetHp() >= target->GetMaxHp();
}
