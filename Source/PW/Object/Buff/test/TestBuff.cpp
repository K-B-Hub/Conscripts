// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Buff/test/TestBuff.h"

UTestBuff::UTestBuff()
{
	accuracy = 20.f;
	critical = 50.f;
}

void UTestBuff::Execute(ACharacterBase* affected)
{
	Super::Execute(affected);
}