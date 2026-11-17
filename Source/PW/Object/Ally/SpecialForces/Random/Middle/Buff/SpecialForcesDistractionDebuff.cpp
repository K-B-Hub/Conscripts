//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Buff/SpecialForcesDistractionDebuff.h"

USpecialForcesDistractionDebuff::USpecialForcesDistractionDebuff()
{
	buffName = NSLOCTEXT("Buff", "SpecialForcesDistractionDebuff_Name", "교란");
	buffDescription = NSLOCTEXT("Buff", "SpecialForcesDistractionDebuff_Description", "엉뚱한 곳에 정신이 팔려 명중이 20 감소한다");

	accuracy = -20.f;

	buffTurn = 2;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
