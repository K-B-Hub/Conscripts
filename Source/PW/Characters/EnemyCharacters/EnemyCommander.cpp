//Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/EnemyCharacters/EnemyCommander.h"

AEnemyCommander::AEnemyCommander()
{
	jobName = NSLOCTEXT("Job", "EnemyCommander_Name", "지휘관");
	characterDescription = NSLOCTEXT("Job", "EnemyCommander_Desc",
		"무전으로 분대를 이끄는 적 지휘관.");

	//초기 스탯, 아군 지휘관과 동일
	maxHp = 30;
	hp = maxHp;
	atk = 12;
	speed = 15;
	skill = 16;
	def = 8;
	mentality = 6;
	sight = 15.f;
	//이동력만 아군보다 높음
	movingPoint = 15.f;
	currentMovingPoint = movingPoint;

	maxBattleResource = 10;
	battleResource = maxBattleResource;

	//성장률
	hpGrowth = 45;
	atkGrowth = 35;
	speedGrowth = 50;
	skillGrowth = 55;
	defGrowth = 50;
	mentalityGrowth = 5;
}
