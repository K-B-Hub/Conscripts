//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Buff/Stress/WeakWillBuff.h"

UWeakWillBuff::UWeakWillBuff()
{
	buffName = NSLOCTEXT("Buff", "WeakWill_Name", "의지 박약");
	buffDescription = NSLOCTEXT("Buff", "WeakWill_Description", "발걸음이 무거워져 나아가지 못한다. 이동력이 9m 감소한다");

	movingPoint = -9.f;

	buffTurn = 2;
	isStackable = false;
}