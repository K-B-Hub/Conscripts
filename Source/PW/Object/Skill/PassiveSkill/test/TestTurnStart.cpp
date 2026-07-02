//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/test/TestTurnStart.h"
#include "Characters/CharacterBase.h"

UTestTurnStart::UTestTurnStart()
{
	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::TurnStart;
}

void UTestTurnStart::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	UE_LOG(LogTemp, Log, TEXT("[Conditional/TurnStart] %s"), o ? *o->GetName() : TEXT("nullowner"));
}
