//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Top/PassiveSkill/TopOptimismPassive.h"

UTopOptimismPassive::UTopOptimismPassive()
{
	skillName = NSLOCTEXT("Skill", "TopOptimism_Name", "극복한 자");
	skillDescription = NSLOCTEXT("Skill", "TopOptimism_Description", "스트레스 극복 시 긍정적인 결과만 발생한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//효과가 on/off라 중복 획득해도 의미 없음
	bAllowDuplicate = false;

	bStressEventAlwaysPositive = true;
}