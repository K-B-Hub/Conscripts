//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/High/Skill/RifleBladeSharpening.h"

URifleBladeSharpening::URifleBladeSharpening()
{
	skillName = NSLOCTEXT("Skill", "RifleBladeSharpening_Name", "칼날 갈기");
	skillDescription = NSLOCTEXT("Skill", "RifleBladeSharpening_Description", "근접 공격을 할 때 치명이 20 증가한다");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	critical = 20.f;
}

bool URifleBladeSharpening::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	//거리가 아니라 스킬 유형으로 판정, 사거리 긴 근접 스킬도 포함
	return skillType == ESkillType::Melee;
}
