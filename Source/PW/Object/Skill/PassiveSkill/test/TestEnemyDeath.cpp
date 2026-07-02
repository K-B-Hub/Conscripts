//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/test/TestEnemyDeath.h"
#include "Characters/CharacterBase.h"

UTestEnemyDeath::UTestEnemyDeath()
{
	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::EnemyDeath;
}

void UTestEnemyDeath::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	UE_LOG(LogTemp, Log, TEXT("[Conditional/EnemyDeath] %s"), o ? *o->GetName() : TEXT("nullowner"));
}
