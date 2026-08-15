//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Stress/Buff/AwakeningBuff.h"

UAwakeningBuff::UAwakeningBuff()
{
	buffName = NSLOCTEXT("Buff", "Awakening_Name", "각성");
	buffDescription = NSLOCTEXT("Buff", "Awakening_Description", "극한의 압박 끝에 감각이 트인다. 명중률·회피율·치명타율이 크게 상승한다");

	accuracy = 50.f;
	evasion = 50.f;
	critical = 50.f;

	buffTurn = 3;
	isStackable = false;
}