//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/CommonUpgrade/Low/LowSkillPassive.h"

ULowSkillPassive::ULowSkillPassive()
{
	skillName = NSLOCTEXT("Skill", "LowSkill_Name", "하급 기술 증가");
	skillDescription = NSLOCTEXT("Skill", "LowSkill_Description", "기술이 1 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	skill = 1;
}
