//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Low/Buff/SniperLegShotDebuff.h"

USniperLegShotDebuff::USniperLegShotDebuff()
{
	buffName = NSLOCTEXT("Buff", "SniperLegShotDebuff_Name", "다리 부상");
	buffDescription = NSLOCTEXT("Buff", "SniperLegShotDebuff_Description", "다리를 맞아 이동력이 3m 감소한다");

	movingPoint = -3.f;

	buffTurn = 2;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
