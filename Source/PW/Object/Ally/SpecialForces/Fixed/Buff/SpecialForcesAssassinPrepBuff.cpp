//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Fixed/Buff/SpecialForcesAssassinPrepBuff.h"

USpecialForcesAssassinPrepBuff::USpecialForcesAssassinPrepBuff()
{
	buffName = NSLOCTEXT("Buff", "SpecialForcesAssassinPrepBuff_Name", "암살 준비");
	buffDescription = NSLOCTEXT("Buff", "SpecialForcesAssassinPrepBuff_Description", "몸을 감춰 회피가 15, 치명이 10 증가한다");

	evasion = 15.f;
	critical = 10.f;

	buffTurn = 2;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
