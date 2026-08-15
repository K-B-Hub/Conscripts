//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Stress/Buff/FlashbackBuff.h"

UFlashbackBuff::UFlashbackBuff()
{
	buffName = NSLOCTEXT("Buff", "Flashback_Name", "플래시백");
	buffDescription = NSLOCTEXT("Buff", "Flashback_Description", "과거의 참상이 눈앞에 겹쳐 보인다. 이동력이 9m, 명중률과 회피율이 40씩 감소한다");

	movingPoint = -9.f;
	accuracy = -40.f;
	evasion = -40.f;

	buffTurn = 2;
	isStackable = false;
}