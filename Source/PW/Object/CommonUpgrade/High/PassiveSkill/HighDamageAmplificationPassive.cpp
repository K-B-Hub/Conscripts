//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/High/PassiveSkill/HighDamageAmplificationPassive.h"

UHighDamageAmplificationPassive::UHighDamageAmplificationPassive()
{
	skillName = NSLOCTEXT("Skill", "HighDamageAmplification_Name", "상급 피해량 증가");
	skillDescription = NSLOCTEXT("Skill", "HighDamageAmplification_Description", "주는 피해량 증가 수치가 6 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	damageAmplification = 6;
}
