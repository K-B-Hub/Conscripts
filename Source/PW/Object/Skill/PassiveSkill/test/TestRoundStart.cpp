// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/test/TestRoundStart.h"
#include "Characters/CharacterBase.h"

UTestRoundStart::UTestRoundStart()
{
	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::RoundStart;
}

void UTestRoundStart::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	UE_LOG(LogTemp, Log, TEXT("[Conditional/RoundStart] %s"), o ? *o->GetName() : TEXT("nullowner"));
}