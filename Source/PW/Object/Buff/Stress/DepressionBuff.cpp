//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Buff/Stress/DepressionBuff.h"

UDepressionBuff::UDepressionBuff()
{
	buffName = NSLOCTEXT("Buff", "Depression_Name", "우울증");
	buffDescription = NSLOCTEXT("Buff", "Depression_Description", "무기력에 잠식된다. 이동력이 5m, 행동력이 1 감소한다");

	movingPoint = -5.f;
	actionPoint = -1;

	buffTurn = 2;
	isStackable = false;
}