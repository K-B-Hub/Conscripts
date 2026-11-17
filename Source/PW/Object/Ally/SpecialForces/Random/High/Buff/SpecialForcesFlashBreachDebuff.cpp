//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/High/Buff/SpecialForcesFlashBreachDebuff.h"

USpecialForcesFlashBreachDebuff::USpecialForcesFlashBreachDebuff()
{
	buffName = NSLOCTEXT("Buff", "SpecialForcesFlashBreachDebuff_Name", "섬광");
	buffDescription = NSLOCTEXT("Buff", "SpecialForcesFlashBreachDebuff_Description", "섬광에 눈이 멀어 명중과 회피가 20 감소한다");

	accuracy = -20.f;
	evasion = -20.f;

	buffTurn = 1;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
