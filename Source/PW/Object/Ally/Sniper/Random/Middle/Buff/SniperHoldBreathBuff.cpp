//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Middle/Buff/SniperHoldBreathBuff.h"

USniperHoldBreathBuff::USniperHoldBreathBuff()
{
	buffName = NSLOCTEXT("Buff", "SniperHoldBreathBuff_Name", "숨 참기");
	buffDescription = NSLOCTEXT("Buff", "SniperHoldBreathBuff_Description", "숨을 참고 조준해 명중이 15, 치명이 10 증가하고 주는 피해가 10% 증가한다");

	accuracy = 15.f;
	critical = 10.f;
	damageAmplification = 10;

	buffTurn = 2;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
