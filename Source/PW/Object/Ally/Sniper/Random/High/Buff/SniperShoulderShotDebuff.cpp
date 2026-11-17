//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/High/Buff/SniperShoulderShotDebuff.h"

USniperShoulderShotDebuff::USniperShoulderShotDebuff()
{
	buffName = NSLOCTEXT("Buff", "SniperShoulderShotDebuff_Name", "어깨 부상");
	buffDescription = NSLOCTEXT("Buff", "SniperShoulderShotDebuff_Description", "어깨를 맞아 명중이 30, 치명이 40 감소한다");

	accuracy = -30.f;
	critical = -40.f;

	buffTurn = 1;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
