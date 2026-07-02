//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/test/TestStatPassive.h"

UTestStatPassive::UTestStatPassive()
{
	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//테스트 스탯 증가량
	hp = 5;
	atk = 3;
	speed = 2;
	def = 2;
	accuracy = 5.0f;
	critical = 3.0f;
}
