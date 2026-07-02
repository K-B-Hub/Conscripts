//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/test/TestAllyDeath.h"
#include "Characters/CharacterBase.h"

UTestAllyDeath::UTestAllyDeath()
{
	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::AllyDeath;
}

void UTestAllyDeath::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	UE_LOG(LogTemp, Log, TEXT("[Conditional/AllyDeath] %s"), o ? *o->GetName() : TEXT("nullowner"));
}
