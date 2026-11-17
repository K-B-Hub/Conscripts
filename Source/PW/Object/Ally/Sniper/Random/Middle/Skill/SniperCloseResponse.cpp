//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Middle/Skill/SniperCloseResponse.h"
#include "Characters/CharacterBase.h"

USniperCloseResponse::USniperCloseResponse()
{
	skillName = NSLOCTEXT("Skill", "SniperCloseResponse_Name", "근거리 대응");
	skillDescription = NSLOCTEXT("Skill", "SniperCloseResponse_Description", "4m 이내의 적 공격 시 명중 +20");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	accuracy = 20.f;
}

bool USniperCloseResponse::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
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
