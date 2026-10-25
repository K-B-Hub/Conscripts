//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Middle/RifleCheekWeld.h"
#include "Characters/CharacterBase.h"

URifleCheekWeld::URifleCheekWeld()
{
	skillName = NSLOCTEXT("Skill", "RifleCheekWeld_Name", "접용점 유지");
	skillDescription = NSLOCTEXT("Skill", "RifleCheekWeld_Description", "이동하지 않고 공격할 때 명중이 15 증가한다");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	accuracy = 15.f;
}

bool URifleCheekWeld::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	const ACharacterBase* o = owner.Get();
	if (!o) return false;

	//이동 여부는 실제 이동력 소모로 판정, 인디케이터의 투기적 이동 토글에 흔들리지 않는다
	return !o->HasConsumedMovingPoint();
}