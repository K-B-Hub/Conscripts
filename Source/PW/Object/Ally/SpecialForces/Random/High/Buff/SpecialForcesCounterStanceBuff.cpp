//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/High/Buff/SpecialForcesCounterStanceBuff.h"

USpecialForcesCounterStanceBuff::USpecialForcesCounterStanceBuff()
{
	buffName = NSLOCTEXT("Buff", "SpecialForcesCounterStanceBuff_Name", "역습 태세");
	buffDescription = NSLOCTEXT("Buff", "SpecialForcesCounterStanceBuff_Description", "반격할 자세를 잡아 명중과 치명이 15 증가한다");

	accuracy = 15.f;
	critical = 15.f;

	buffTurn = 1;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
