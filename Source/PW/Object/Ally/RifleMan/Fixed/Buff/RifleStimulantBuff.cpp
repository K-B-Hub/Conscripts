//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Fixed/Buff/RifleStimulantBuff.h"

URifleStimulantBuff::URifleStimulantBuff()
{
	buffName = NSLOCTEXT("Buff", "RifleStimulant_Name", "각성제");
	buffDescription = NSLOCTEXT("Buff", "RifleStimulant_Description", "명중·회피·치명이 10 증가하고 주는 피해가 10% 증가, 받는 피해가 10% 감소한다");

	accuracy = 10.f;
	evasion = 10.f;
	critical = 10.f;
	damageAmplification = 10;
	damageReduction = 10;

	buffTurn = 1;
	isStart = false;
	isEnd = false;
	isStackable = false;
}
