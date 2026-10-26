//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Middle/Skill/RifleCQB.h"

URifleCQB::URifleCQB()
{
	skillName = NSLOCTEXT("Skill", "RifleCQB_Name", "CQB");
	skillDescription = NSLOCTEXT("Skill", "RifleCQB_Description", "근접 공격을 할 때 명중이 10 증가한다");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	accuracy = 10.f;
}

bool URifleCQB::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	//거리가 아니라 스킬 유형으로 판정, 사거리 긴 근접 스킬도 포함
	return skillType == ESkillType::Melee;
}