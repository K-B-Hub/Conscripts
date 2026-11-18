//Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/AllyCharacters/SpecialForces.h"

ASpecialForces::ASpecialForces()
{
	jobName = NSLOCTEXT("Job", "SpecialForces_Name", "특수부대원");

	//직업 설명이라 파생 캐릭터 BP가 모두 상속한다, 캐릭터마다 다르게 하려면 BP에서 덮어쓴다
	characterDescription = NSLOCTEXT("Job", "SpecialForces_Desc",
		"적진 깊숙이 파고들어 고립된 적을 처치하는 암살자.\n"
		"이동력과 치명타율, 회피율이 높다.\n");

	//초기 스탯
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

	//고유 자원은 장비(탄창, 연막, 폭약 등 휴대 장비 묶음)
	maxBattleResource = 8;
	battleResource = maxBattleResource;

	//성장률, 레벨업마다 1~100 굴림이 이 값 이하면 해당 스탯이 1 오른다
	hpGrowth = 35;
	atkGrowth = 50;
	speedGrowth = 70;
	skillGrowth = 55;
	defGrowth = 25;
	mentalityGrowth = 5;
}
