//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Top/PassiveSkill/TopActionPointPassive.h"

UTopActionPointPassive::UTopActionPointPassive()
{
	skillName = NSLOCTEXT("Skill", "TopActionPoint_Name", "근면성실");
	skillDescription = NSLOCTEXT("Skill", "TopActionPoint_Description", "행동력이 2 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	actionPoint = 2;
}