//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Top/PassiveSkill/TopDamageReductionPassive.h"

UTopDamageReductionPassive::UTopDamageReductionPassive()
{
	skillName = NSLOCTEXT("Skill", "TopDamageReduction_Name", "금강불괴");
	skillDescription = NSLOCTEXT("Skill", "TopDamageReduction_Description", "받는 피해량 감소 수치가 20 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	damageReduction = 20;
}