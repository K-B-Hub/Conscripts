//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Middle/Skill/RifleMeleeAmateur.h"

URifleMeleeAmateur::URifleMeleeAmateur()
{
	skillName = NSLOCTEXT("Skill", "RifleMeleeAmateur_Name", "근접전 아마추어");
	skillDescription = NSLOCTEXT("Skill", "RifleMeleeAmateur_Description", "근접 공격의 공격력이 5 증가한다");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	atk = 5;
}

bool URifleMeleeAmateur::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	return skillType == ESkillType::Melee;
}