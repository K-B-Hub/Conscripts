//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Basic/SniperIdentity.h"
#include "Characters/CharacterBase.h"

USniperIdentity::USniperIdentity()
{
	skillName = NSLOCTEXT("Skill", "SniperIdentity_Name", "저격수");
	skillDescription = NSLOCTEXT("Skill", "SniperIdentity_Description", "4m 이내의 적 공격 시 명중 -10, 치명 -20");

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	accuracy = -10.f;
	critical = -20.f;
}

bool USniperIdentity::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
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
