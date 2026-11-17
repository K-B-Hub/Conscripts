//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Fixed/Buff/SniperHoldBreathBuff.h"

USniperHoldBreathBuff::USniperHoldBreathBuff()
{
	buffName = NSLOCTEXT("Buff", "SniperHoldBreathBuff_Name", "숨 참기");
	buffDescription = NSLOCTEXT("Buff", "SniperHoldBreathBuff_Description", "숨을 참고 조준해 명중이 30, 치명이 20 증가하고 주는 피해가 20% 증가한다");

	accuracy = 30.f;
	critical = 20.f;
	damageAmplification = 20;

	buffTurn = 2;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
