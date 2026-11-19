//Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/AllyCharacters/Commander.h"

ACommander::ACommander()
{
	jobName = NSLOCTEXT("Job", "Commander_Name", "지휘관");

	//직업 설명이라 파생 캐릭터 BP가 모두 상속한다, 캐릭터마다 다르게 하려면 BP에서 덮어쓴다
	characterDescription = NSLOCTEXT("Job", "Commander_Desc",
		"무전으로 분대를 이끄는 지휘관.\n"
		"공격력은 낮지만 주변 아군을 강화하고 적을 흔들며, 압박에 강하다.");

	//초기 스탯
	maxHp = 30;
	hp = maxHp;
	atk = 12;
	speed = 15;
	skill = 16;
	def = 8;
	mentality = 6;
	sight = 15.f;
	movingPoint = 9.f;
	currentMovingPoint = movingPoint;

	maxBattleResource = 10;
	battleResource = maxBattleResource;

	//성장률, 레벨업마다 1~100 굴림이 이 값 이하면 해당 스탯이 1 오른다
	hpGrowth = 45;
	atkGrowth = 35;
	speedGrowth = 50;
	skillGrowth = 55;
	defGrowth = 50;
	mentalityGrowth = 5;
}
