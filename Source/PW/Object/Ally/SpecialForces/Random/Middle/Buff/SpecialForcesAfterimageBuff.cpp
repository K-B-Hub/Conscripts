//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Buff/SpecialForcesAfterimageBuff.h"

USpecialForcesAfterimageBuff::USpecialForcesAfterimageBuff()
{
	buffName = NSLOCTEXT("Buff", "SpecialForcesAfterimageBuff_Name", "잔영");
	buffDescription = NSLOCTEXT("Buff", "SpecialForcesAfterimageBuff_Description", "피할수록 발이 빨라져 이동력이 2m 증가한다(최대 3중첩)");

	movingPoint = 2.f;

	buffTurn = 1;
	isStart = false;
	isEnd = false;
	isStackable = true;
	maxStack = 3;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
