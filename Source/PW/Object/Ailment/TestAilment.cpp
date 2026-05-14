// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Ailment/TestAilment.h"
#include "Characters/CharacterBase.h"

UTestAilment::UTestAilment()
{
	ailmentTurn = 3;
	isStart = true;
	isEnd = false;
	isStackable = false;
}

void UTestAilment::Execute(ACharacterBase* affected)
{
	Super::Execute(affected);

	if (!affected) return;

	// 방어력 무시 고정 피해 — ReceiveDamage는 인자 그대로 hp에 가산되므로 def 우회
	affected->ReceiveDamage(5, false);
}