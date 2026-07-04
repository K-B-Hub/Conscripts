// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Buff/test/TestDebuff.h"

UTestDebuff::UTestDebuff()
{
	def = -5;
	atk = -5;
}

void UTestDebuff::Execute(ACharacterBase* affected)
{
	Super::Execute(affected);
}
