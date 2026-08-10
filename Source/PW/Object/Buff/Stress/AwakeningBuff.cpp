//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Buff/Stress/AwakeningBuff.h"

UAwakeningBuff::UAwakeningBuff()
{
	accuracy = 50.f;
	evasion = 50.f;
	critical = 50.f;

	buffTurn = 3;
	isStackable = false;
}