//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/test/TestDamaged.h"
#include "Characters/CharacterBase.h"

UTestDamaged::UTestDamaged()
{
	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::Damaged;
}

void UTestDamaged::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	UE_LOG(LogTemp, Log, TEXT("[Conditional/Damaged] %s (hp=%d/%d)"),
		o ? *o->GetName() : TEXT("nullowner"),
		o ? o->GetHp() : 0,
		o ? o->GetMaxHp() : 0);
}
