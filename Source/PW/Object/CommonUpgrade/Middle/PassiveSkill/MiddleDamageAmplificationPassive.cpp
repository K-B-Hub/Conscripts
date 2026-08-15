//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Middle/PassiveSkill/MiddleDamageAmplificationPassive.h"

UMiddleDamageAmplificationPassive::UMiddleDamageAmplificationPassive()
{
	skillName = NSLOCTEXT("Skill", "MiddleDamageAmplification_Name", "피해량 증가");
	skillDescription = NSLOCTEXT("Skill", "MiddleDamageAmplification_Description", "주는 피해량 증가 수치가 3 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	damageAmplification = 3;
}
