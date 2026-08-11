//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Buff/test/TestAtkBuff.h"

UTestAtkBuff::UTestAtkBuff()
{
	buffName = NSLOCTEXT("Buff", "TestAtk_Name", "테스트 공격력 버프");
	buffDescription = NSLOCTEXT("Buff", "TestAtk_Description", "테스트용 버프. 공격력이 5 상승한다");

	atk = 5;
	buffTurn = 3;
	isStackable = false;
}
