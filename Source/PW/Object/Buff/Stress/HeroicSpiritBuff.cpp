//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Buff/Stress/HeroicSpiritBuff.h"

UHeroicSpiritBuff::UHeroicSpiritBuff()
{
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