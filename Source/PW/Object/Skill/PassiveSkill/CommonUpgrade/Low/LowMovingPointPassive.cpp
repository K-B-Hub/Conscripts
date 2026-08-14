//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/CommonUpgrade/Low/LowMovingPointPassive.h"

ULowMovingPointPassive::ULowMovingPointPassive()
{
	skillName = NSLOCTEXT("Skill", "LowMovingPoint_Name", "하급 이동력 증가");
	skillDescription = NSLOCTEXT("Skill", "LowMovingPoint_Description", "이동력이 1 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	movingPoint = 1.0f;
}
