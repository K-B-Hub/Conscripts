//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Middle/RifleMoraleBuff.h"

URifleMoraleBuff::URifleMoraleBuff()
{
	buffName = NSLOCTEXT("Buff", "RifleMorale_Name", "사기");
	buffDescription = NSLOCTEXT("Buff", "RifleMorale_Description", "사기가 올라 명중이 10 증가한다");

	accuracy = 10.f;

	buffTurn = 2;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//명중 상승의 가치는 대상 명중·상대 회피에 달려 있어 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}