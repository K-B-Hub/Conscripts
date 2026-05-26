// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/test/TestBeforeCalc.h"

#include "../../../../Characters/CharacterBase.h"

UTestBeforeCalc::UTestBeforeCalc()
{
	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	atk = 8;
	damageAmplification = 0;
	penetration = 0;
	accuracy = 0.0f;
	critical = 0.0f;
}

bool UTestBeforeCalc::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType)
{
	if (!target) return false;
	
	int targetHp = target->GetHp();
	int targetMaxHp = target->GetMaxHp();
	
	if (targetHp > 0 && targetHp <= targetMaxHp / 2)
	{
		return true;
	}
	
	return false;
}