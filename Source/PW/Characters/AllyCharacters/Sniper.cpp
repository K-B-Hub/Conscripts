//Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/AllyCharacters/Sniper.h"

ASniper::ASniper()
{
	jobName = NSLOCTEXT("Job", "Sniper_Name", "저격수");

	//직업 설명이라 파생 캐릭터 BP가 모두 상속한다, 캐릭터마다 다르게 하려면 BP에서 덮어쓴다
	characterDescription = NSLOCTEXT("Job", "Sniper_Desc",
		"조준경 너머 먼 거리의 한 명을 노리는 초원거리 저격수.\n"
		"전투 자원이 적고 방어력이 떨어지지만 명중률과 치명타율이 높다.");

	//초기 스탯
	maxHp = 26;
	hp = maxHp;
	atk = 16;
	speed = 11;
	skill = 20;
	def = 6;
	mentality = 2;
	sight = 22.f;
	movingPoint = 5.f;
	currentMovingPoint = movingPoint;

	//고유 자원은 탄환
	maxBattleResource = 4;
	battleResource = maxBattleResource;

	//성장률, 레벨업마다 1~100 굴림이 이 값 이하면 해당 스탯이 1 오른다
	hpGrowth = 40;
	atkGrowth = 55;
	speedGrowth = 35;
	skillGrowth = 70;
	defGrowth = 35;
	mentalityGrowth = 5;
}
