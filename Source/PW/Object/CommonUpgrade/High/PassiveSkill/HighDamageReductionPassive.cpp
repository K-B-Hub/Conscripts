//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/High/PassiveSkill/HighDamageReductionPassive.h"

UHighDamageReductionPassive::UHighDamageReductionPassive()
{
	skillName = NSLOCTEXT("Skill", "HighDamageReduction_Name", "상급 피해량 감소");
	skillDescription = NSLOCTEXT("Skill", "HighDamageReduction_Description", "받는 피해량 감소 수치가 6 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	damageReduction = 6;
}
