//Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/EnemyCharacters/EnemySniper.h"

AEnemySniper::AEnemySniper()
{
	jobName = NSLOCTEXT("Job", "EnemySniper_Name", "저격수");
	characterDescription = NSLOCTEXT("Job", "EnemySniper_Desc",
		"먼 거리에서 조준경으로 노리는 적 저격수.");

	//초기 스탯, 아군 저격수와 동일
	maxHp = 26;
	hp = maxHp;
	atk = 16;
	speed = 11;
	skill = 20;
	def = 6;
	mentality = 2;
	sight = 22.f;
	movingPoint = 7.f;
	currentMovingPoint = movingPoint;

	//고유 자원은 탄환
	maxBattleResource = 4;
	battleResource = maxBattleResource;

	//성장률
	hpGrowth = 40;
	atkGrowth = 55;
	speedGrowth = 35;
	skillGrowth = 70;
	defGrowth = 35;
	mentalityGrowth = 5;
}
