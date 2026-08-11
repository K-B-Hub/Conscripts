//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Buff/Stress/AnxietyBuff.h"

UAnxietyBuff::UAnxietyBuff()
{
	buffName = NSLOCTEXT("Buff", "Anxiety_Name", "불안장애");
	buffDescription = NSLOCTEXT("Buff", "Anxiety_Description", "끊임없는 불안이 손끝을 떨게 한다. 명중률과 회피율이 60씩 감소한다");

	accuracy = -60.f;
	evasion = -60.f;

	buffTurn = 2;
	isStackable = false;
}