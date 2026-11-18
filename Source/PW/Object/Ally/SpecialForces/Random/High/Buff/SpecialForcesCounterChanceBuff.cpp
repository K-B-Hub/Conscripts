//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/High/Buff/SpecialForcesCounterChanceBuff.h"

USpecialForcesCounterChanceBuff::USpecialForcesCounterChanceBuff()
{
	buffName = NSLOCTEXT("Buff", "SpecialForcesCounterChanceBuff_Name", "역공의 기회");
	buffDescription = NSLOCTEXT("Buff", "SpecialForcesCounterChanceBuff_Description", "빗나간 총알 뒤로 틈을 봐 명중과 치명이 10 증가한다");

	accuracy = 10.f;
	critical = 10.f;

	buffTurn = 1;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
