//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/test/TestMoveComplete.h"
#include "Characters/CharacterBase.h"

UTestMoveComplete::UTestMoveComplete()
{
	skillName = NSLOCTEXT("Skill", "TestMoveComplete_Name", "테스트");
	skillDescription = NSLOCTEXT("Skill", "TestMoveComplete_Description", "123123");
	
	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::MoveComplete;
}

void UTestMoveComplete::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	UE_LOG(LogTemp, Log, TEXT("[Conditional/MoveComplete] %s"), o ? *o->GetName() : TEXT("nullowner"));
}
