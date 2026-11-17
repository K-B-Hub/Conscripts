//Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/EnemyCharacters/EnemySpecialForces.h"

AEnemySpecialForces::AEnemySpecialForces()
{
	jobName = NSLOCTEXT("Job", "EnemySpecialForces_Name", "특수부대원");
	characterDescription = NSLOCTEXT("Job", "EnemySpecialForces_Desc",
		"소음 카빈을 들고 깊숙이 파고드는 적 특수부대원.");

	//초기 스탯, 아군 특수부대원과 동일
	maxHp = 24;
	hp = maxHp;
	atk = 15;
	speed = 20;
	skill = 17;
	def = 5;
	mentality = 4;
	sight = 10.f;
	movingPoint = 8.f;
	currentMovingPoint = movingPoint;

	//고유 자원은 장비
	maxBattleResource = 8;
	battleResource = maxBattleResource;

	//성장률
	hpGrowth = 35;
	atkGrowth = 50;
	speedGrowth = 70;
	skillGrowth = 55;
	defGrowth = 25;
	mentalityGrowth = 5;
}
