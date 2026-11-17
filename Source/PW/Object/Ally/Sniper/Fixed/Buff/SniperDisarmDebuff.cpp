//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Fixed/Buff/SniperDisarmDebuff.h"

USniperDisarmDebuff::USniperDisarmDebuff()
{
	buffName = NSLOCTEXT("Buff", "SniperDisarmDebuff_Name", "손 부상");
	buffDescription = NSLOCTEXT("Buff", "SniperDisarmDebuff_Description", "총을 쥔 손을 맞아 명중이 30 감소한다");

	accuracy = -30.f;

	buffTurn = 1;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
