//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Middle/PassiveSkill/MiddleMovingPointPassive.h"

UMiddleMovingPointPassive::UMiddleMovingPointPassive()
{
	skillName = NSLOCTEXT("Skill", "MiddleMovingPoint_Name", "중급 이동력 증가");
	skillDescription = NSLOCTEXT("Skill", "MiddleMovingPoint_Description", "이동력이 2 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	movingPoint = 2.0f;
}
