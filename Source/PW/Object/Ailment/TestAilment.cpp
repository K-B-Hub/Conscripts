//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Ailment/TestAilment.h"
#include "Characters/CharacterBase.h"

UTestAilment::UTestAilment()
{
	ailmentTurn = 3;
	isStart = true;
	isEnd = false;
	isStackable = false;

	aiDamagePerTurn = 2.f;
	aiControlCoefficient = 0.f;
}

void UTestAilment::Execute(ACharacterBase* affected)
{
	Super::Execute(affected);

	if (!affected) return;

	//방어력 무시 고정 피해
	affected->ReceiveDamage(2, false);
}
