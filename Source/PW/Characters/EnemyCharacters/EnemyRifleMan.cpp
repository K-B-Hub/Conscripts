//Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/EnemyCharacters/EnemyRifleMan.h"

AEnemyRifleMan::AEnemyRifleMan()
{
	jobName = NSLOCTEXT("Job", "EnemyRifleMan_Name", "소총수");
	characterDescription = NSLOCTEXT("Job", "EnemyRifleMan_Desc",
		"제식 소총으로 무장한 적 보병.");

	//초기 스탯, 아군 소총수와 동일
	maxHp = 32;
	hp = maxHp;
	atk = 13;
	speed = 14;
	skill = 16;
	def = 9;
	mentality = 3;
	sight = 15.f;
	movingPoint = 6.f;
	currentMovingPoint = movingPoint;

	//고유 자원은 탄창
	maxBattleResource = 12;
	battleResource = maxBattleResource;

	//성장률
	hpGrowth = 50;
	atkGrowth = 45;
	speedGrowth = 45;
	skillGrowth = 55;
	defGrowth = 40;
	mentalityGrowth = 5;
}
