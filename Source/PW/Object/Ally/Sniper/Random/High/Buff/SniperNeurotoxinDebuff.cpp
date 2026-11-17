//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/High/Buff/SniperNeurotoxinDebuff.h"

USniperNeurotoxinDebuff::USniperNeurotoxinDebuff()
{
	buffName = NSLOCTEXT("Buff", "SniperNeurotoxinDebuff_Name", "신경독");
	buffDescription = NSLOCTEXT("Buff", "SniperNeurotoxinDebuff_Description", "신경독이 퍼져 이동력이 2m 감소한다");

	movingPoint = -2.f;

	buffTurn = 2;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
