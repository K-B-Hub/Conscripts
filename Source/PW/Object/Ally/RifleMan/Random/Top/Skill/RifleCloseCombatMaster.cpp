//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Top/Skill/RifleCloseCombatMaster.h"

URifleCloseCombatMaster::URifleCloseCombatMaster()
{
	skillName = NSLOCTEXT("Skill", "RifleCloseCombatMaster_Name", "근접전의 달인");
	skillDescription = NSLOCTEXT("Skill", "RifleCloseCombatMaster_Description", "근접 공격을 할 때 치명이 20, 주는 피해가 20% 증가한다");
	skillGrade = EUpgradeGrade::Top;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	critical = 20.f;
	damageAmplification = 20;
}

bool URifleCloseCombatMaster::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	//거리가 아니라 스킬 유형으로 판정, 사거리 긴 근접 스킬도 포함
	return skillType == ESkillType::Melee;
}
