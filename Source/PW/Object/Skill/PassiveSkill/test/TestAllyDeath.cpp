//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/test/TestAllyDeath.h"
#include "Characters/CharacterBase.h"

UTestAllyDeath::UTestAllyDeath()
{
	skillName = NSLOCTEXT("Skill", "TestAllyDeath_Name", "테스트 동료 사망");
	skillDescription = NSLOCTEXT("Skill", "TestAllyDeath_Description", "123123");
	
	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::AllyDeath;
}

void UTestAllyDeath::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	UE_LOG(LogTemp, Log, TEXT("[Conditional/AllyDeath] %s"), o ? *o->GetName() : TEXT("nullowner"));
}
