//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/High/PassiveSkill/HighActionPointPassive.h"

UHighActionPointPassive::UHighActionPointPassive()
{
	skillName = NSLOCTEXT("Skill", "HighActionPoint_Name", "행동력 증가");
	skillDescription = NSLOCTEXT("Skill", "HighActionPoint_Description", "행동력이 1 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	actionPoint = 1;
}
