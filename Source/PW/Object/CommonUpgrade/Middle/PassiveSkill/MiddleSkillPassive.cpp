//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Middle/PassiveSkill/MiddleSkillPassive.h"

UMiddleSkillPassive::UMiddleSkillPassive()
{
	skillName = NSLOCTEXT("Skill", "MiddleSkill_Name", "중급 기술 증가");
	skillDescription = NSLOCTEXT("Skill", "MiddleSkill_Description", "기술이 2 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	skill = 2;
}
