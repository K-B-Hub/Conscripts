// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Buff/test/TestDebuff.h"

UTestDebuff::UTestDebuff()
{
	buffName = NSLOCTEXT("Buff", "TestDebuff_Name", "테스트 디버프");
	buffDescription = NSLOCTEXT("Buff", "TestDebuff_Description", "테스트용 디버프. 공격력과 방어력이 각각 5 감소한다");

	def = -5;
	atk = -5;
}

void UTestDebuff::Execute(ACharacterBase* affected)
{
	Super::Execute(affected);
}
