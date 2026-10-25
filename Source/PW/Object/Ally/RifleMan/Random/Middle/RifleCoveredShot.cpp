//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Middle/RifleCoveredShot.h"
#include "Characters/CharacterBase.h"

URifleCoveredShot::URifleCoveredShot()
{
	skillName = NSLOCTEXT("Skill", "RifleCoveredShot_Name", "엄폐 사격");
	skillDescription = NSLOCTEXT("Skill", "RifleCoveredShot_Description", "포복 중에 공격할 때 명중과 치명이 10 증가한다");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	accuracy = 10.f;
	critical = 10.f;
}

bool URifleCoveredShot::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	const ACharacterBase* o = owner.Get();
	if (!o) return false;

	return o->IsProne();
}