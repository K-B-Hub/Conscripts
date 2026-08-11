//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Buff/Stress/HeroicSpiritBuff.h"

UHeroicSpiritBuff::UHeroicSpiritBuff()
{
	buffName = NSLOCTEXT("Buff", "HeroicSpirit_Name", "영웅의 기개");
	buffDescription = NSLOCTEXT("Buff", "HeroicSpirit_Description", "두려움을 딛고 일어선다. 공격력·속도·기술·방어력을 비롯한 주요 전투 스탯이 일제히 상승한다");

	atk = 10;
	speed = 10;
	skill = 10;
	def = 10;
	damageReduction = 10;
	damageAmplification = 10;
	penetration = 10;

	buffTurn = 3;
	isStackable = false;
}