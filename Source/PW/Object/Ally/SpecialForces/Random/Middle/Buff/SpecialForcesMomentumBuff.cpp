//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Buff/SpecialForcesMomentumBuff.h"

USpecialForcesMomentumBuff::USpecialForcesMomentumBuff()
{
	buffName = NSLOCTEXT("Buff", "SpecialForcesMomentumBuff_Name", "고조");
	buffDescription = NSLOCTEXT("Buff", "SpecialForcesMomentumBuff_Description", "몸이 가벼워져 회피가 25 증가한다");

	evasion = 25.f;

	buffTurn = 2;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
