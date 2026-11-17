//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/High/Buff/SniperExposeWeaknessDebuff.h"

USniperExposeWeaknessDebuff::USniperExposeWeaknessDebuff()
{
	buffName = NSLOCTEXT("Buff", "SniperExposeWeaknessDebuff_Name", "약점 노출");
	buffDescription = NSLOCTEXT("Buff", "SniperExposeWeaknessDebuff_Description", "드러난 약점 때문에 받는 피해가 10% 증가한다");

	damageReduction = -10;

	buffTurn = 2;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}
