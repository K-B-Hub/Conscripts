//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Middle/Skill/SniperLongRangePrecision.h"
#include "Characters/CharacterBase.h"

USniperLongRangePrecision::USniperLongRangePrecision()
{
	skillName = NSLOCTEXT("Skill", "SniperLongRangePrecision_Name", "원거리 정밀 조준");
	skillDescription = NSLOCTEXT("Skill", "SniperLongRangePrecision_Description", "15m 이상 떨어진 적 공격 시 주는 피해 +15%");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	damageAmplification = 15;
}

bool USniperLongRangePrecision::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	if (!target) return false;
	ACharacterBase* o = owner.Get();
	if (!o) return false;

	//아군 대상 스킬은 제외
	if (o->IsAlly() == target->IsAlly()) return false;

	//높이차는 무시, 수평 거리로 측정
	const float dist = FVector::Dist2D(attackerLocation, target->GetActorLocation());
	return dist >= farRange;
}
