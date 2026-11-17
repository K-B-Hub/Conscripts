//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/High/Buff/SpecialForcesChainCriticalBuff.h"

USpecialForcesChainCriticalBuff::USpecialForcesChainCriticalBuff()
{
	buffName = NSLOCTEXT("Buff", "SpecialForcesChainCriticalBuff_Name", "연쇄 치명");
	buffDescription = NSLOCTEXT("Buff", "SpecialForcesChainCriticalBuff_Description", "급소가 더 잘 보여 치명이 10 증가한다(최대 3중첩)");

	critical = 10.f;

	buffTurn = 2;
	isStart = false;
	isEnd = false;
	isStackable = true;
	maxStack = 3;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
