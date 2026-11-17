//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/High/Buff/SpecialForcesFragRoundDebuff.h"

USpecialForcesFragRoundDebuff::USpecialForcesFragRoundDebuff()
{
	buffName = NSLOCTEXT("Buff", "SpecialForcesFragRoundDebuff_Name", "방호 파쇄");
	buffDescription = NSLOCTEXT("Buff", "SpecialForcesFragRoundDebuff_Description", "탄두 파편에 방호가 뚫려 방어가 8 감소한다");

	def = -8;

	buffTurn = 2;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
