//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/test/TestTurnEnd.h"
#include "Characters/CharacterBase.h"

UTestTurnEnd::UTestTurnEnd()
{
	skillName = NSLOCTEXT("Skill", "TestTurnEnd_Name", "테스트");
	skillDescription = NSLOCTEXT("Skill", "TestTurnEnd_Description", "123123");	
	
	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::TurnEnd;
}

void UTestTurnEnd::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	UE_LOG(LogTemp, Log, TEXT("[Conditional/TurnEnd] %s"), o ? *o->GetName() : TEXT("nullowner"));
}
