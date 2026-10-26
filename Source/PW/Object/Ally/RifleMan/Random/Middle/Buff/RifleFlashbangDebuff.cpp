//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Middle/Buff/RifleFlashbangDebuff.h"

URifleFlashbangDebuff::URifleFlashbangDebuff()
{
	buffName = NSLOCTEXT("Buff", "RifleFlashbang_Name", "실명");
	buffDescription = NSLOCTEXT("Buff", "RifleFlashbang_Description", "눈이 멀어 명중과 회피가 15 감소한다");

	accuracy = -15.f;
	evasion = -15.f;

	buffTurn = 2;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//명중·회피 감소는 상대 공격력에 따라 가치가 달라져 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}