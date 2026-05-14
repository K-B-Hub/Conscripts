// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/test/TestAfterDamage.h"

UTestAfterDamage::UTestAfterDamage()
{
	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterDamage;
	conditionType = EConditionalType::None;
}

void UTestAfterDamage::Execute_AfterDamage(ACharacterBase* target)
{
	UE_LOG(LogTemp, Log, TEXT("[TestAfterDamage] %s의 AfterDamage 패시브 발동"), *GetName());
}