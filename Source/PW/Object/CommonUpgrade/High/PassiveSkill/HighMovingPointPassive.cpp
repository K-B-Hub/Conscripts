//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/High/PassiveSkill/HighMovingPointPassive.h"

UHighMovingPointPassive::UHighMovingPointPassive()
{
	skillName = NSLOCTEXT("Skill", "HighMovingPoint_Name", "상급 이동력 증가");
	skillDescription = NSLOCTEXT("Skill", "HighMovingPoint_Description", "이동력이 3 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	movingPoint = 3.0f;
}
