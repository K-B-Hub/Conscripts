//Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/AllyCharacters/RifleMan.h"

ARifleMan::ARifleMan()
{
	jobName = NSLOCTEXT("Job", "RifleMan_Name", "소총수");

	//직업 설명이라 파생 캐릭터 BP가 모두 상속한다, 캐릭터마다 다르게 하려면 BP에서 덮어쓴다
	characterDescription = NSLOCTEXT("Job", "RifleMan_Desc",
		"제식 소총으로 중거리를 맡는 보병의 기본.\n"
		"제압사격으로 적의 고개를 숙이게 만들고, 거리가 무너지면 총검으로 맞선다.\n"
		"탄창을 비우면 쏠 수 없으니 재장전 시점을 스스로 골라야 한다.");

	//초기 스탯
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

	//성장률, 레벨업마다 1~100 굴림이 이 값 이하면 해당 스탯이 1 오른다
	hpGrowth = 50;
	atkGrowth = 45;
	speedGrowth = 45;
	skillGrowth = 55;
	defGrowth = 40;
	mentalityGrowth = 5;
}
