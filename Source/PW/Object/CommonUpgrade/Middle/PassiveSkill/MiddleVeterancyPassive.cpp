//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Middle/PassiveSkill/MiddleVeterancyPassive.h"
#include "Characters/CharacterBase.h"

UMiddleVeterancyPassive::UMiddleVeterancyPassive()
{
	skillName = NSLOCTEXT("Skill", "MiddleVeterancy_Name", "노련함");
	skillDescription = NSLOCTEXT("Skill", "MiddleVeterancy_Description", "8m 밖의 적을 공격할 때 공격력이 3, 명중이 10 증가한다");

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	atk = 3;
	accuracy = 10.0f;
}

bool UMiddleVeterancyPassive::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	if (!target) return false;

	const ACharacterBase* o = owner.Get();
	if (!o) return false;

	//아군 대상 스킬은 제외
	if (o->IsAlly() == target->IsAlly()) return false;

	//높이차는 무시, 시야 판정과 동일하게 수평 거리로 측정
	return FVector::Dist2D(attackerLocation, target->GetActorLocation()) > rangedThreshold;
}