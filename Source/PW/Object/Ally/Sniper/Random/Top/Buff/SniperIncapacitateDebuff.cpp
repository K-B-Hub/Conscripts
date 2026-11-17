//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Top/Buff/SniperIncapacitateDebuff.h"

USniperIncapacitateDebuff::USniperIncapacitateDebuff()
{
	buffName = NSLOCTEXT("Buff", "SniperIncapacitateDebuff_Name", "무력화");
	buffDescription = NSLOCTEXT("Buff", "SniperIncapacitateDebuff_Description", "급소를 맞아 행동력이 2 감소한다");

	actionPoint = -2;

	buffTurn = 1;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
