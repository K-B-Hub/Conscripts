//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Fixed/Skill/SpecialForcesCQBExpert.h"
#include "Characters/CharacterBase.h"

USpecialForcesCQBExpert::USpecialForcesCQBExpert()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesCQBExpert_Name", "CQB 전문가");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesCQBExpert_Description", "6m 이내의 적 공격 시 주는 피해 +10%");

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	damageAmplification = 10;
}

bool USpecialForcesCQBExpert::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	if (!target) return false;
	ACharacterBase* o = owner.Get();
	if (!o) return false;

	//아군 대상 스킬은 제외
	if (o->IsAlly() == target->IsAlly()) return false;

	//높이차는 무시, 수평 거리로 측정
	const float dist = FVector::Dist2D(attackerLocation, target->GetActorLocation());
	return dist <= closeRange;
}
