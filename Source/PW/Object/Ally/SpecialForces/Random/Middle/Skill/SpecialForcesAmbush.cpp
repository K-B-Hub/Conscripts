//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Skill/SpecialForcesAmbush.h"
#include "Characters/CharacterBase.h"

USpecialForcesAmbush::USpecialForcesAmbush()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesAmbush_Name", "급습");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesAmbush_Description", "이번 턴에 이동한 뒤 4m 이내의 적 공격 시 명중 +10, 주는 피해 +10%");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	accuracy = 10.f;
	damageAmplification = 10;
}

bool USpecialForcesAmbush::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	if (!target) return false;
	ACharacterBase* o = owner.Get();
	if (!o) return false;

	//아군 대상 스킬은 제외
	if (o->IsAlly() == target->IsAlly()) return false;

	//isMoved는 인디케이터가 투기적으로 켜므로 실제 이동력 소모로 판정
	if (!o->HasConsumedMovingPoint()) return false;

	//높이차는 무시, 수평 거리로 측정
	const float dist = FVector::Dist2D(attackerLocation, target->GetActorLocation());
	return dist <= closeRange;
}
