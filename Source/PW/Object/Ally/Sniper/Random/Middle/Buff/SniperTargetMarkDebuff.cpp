//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Middle/Buff/SniperTargetMarkDebuff.h"

USniperTargetMarkDebuff::USniperTargetMarkDebuff()
{
	buffName = NSLOCTEXT("Buff", "SniperTargetMarkDebuff_Name", "조준 표식");
	buffDescription = NSLOCTEXT("Buff", "SniperTargetMarkDebuff_Description", "표식이 붙어 회피가 15 감소한다");

	evasion = -15.f;

	buffTurn = 2;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
