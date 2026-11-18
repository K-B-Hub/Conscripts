//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/High/Buff/SpecialForcesBlurBuff.h"

USpecialForcesBlurBuff::USpecialForcesBlurBuff()
{
	buffName = NSLOCTEXT("Buff", "SpecialForcesBlurBuff_Name", "잔상");
	buffDescription = NSLOCTEXT("Buff", "SpecialForcesBlurBuff_Description", "움직임이 흐려져 회피가 5 증가한다(최대 3중첩)");

	evasion = 5.f;

	buffTurn = 2;
	isStart = false;
	isEnd = false;
	isStackable = true;
	maxStack = 3;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
