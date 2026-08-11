//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Buff/test/TestBuff.h"

UTestBuff::UTestBuff()
{
	buffName = NSLOCTEXT("Buff", "Test_Name", "테스트 버프");
	buffDescription = NSLOCTEXT("Buff", "Test_Description", "테스트용 버프. 명중률과 치명타율이 상승한다");

	accuracy = 20.f;
	critical = 50.f;
}

void UTestBuff::Execute(ACharacterBase* affected)
{
	Super::Execute(affected);
}
