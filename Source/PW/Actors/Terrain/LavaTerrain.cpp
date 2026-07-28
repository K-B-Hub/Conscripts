//Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Terrain/LavaTerrain.h"
#include "Characters/CharacterBase.h"

void ALavaTerrain::BeginPlay()
{
	Super::BeginPlay();

	//damagePerTurn이 손익의 단일 출처, AI 평가값은 여기서 파생 (턴 시작·종료 2회)
	aiBaseValue = -damagePerTurn * 2.f;
}

void ALavaTerrain::OnCharacterTurnStart(ACharacterBase* character)
{
	//환경 피해는 비치명, hp 1까지만 깎임
	if (character) character->ReceiveDamage(damagePerTurn, false);
}

void ALavaTerrain::OnCharacterTurnEnd(ACharacterBase* character)
{
	if (character) character->ReceiveDamage(damagePerTurn, false);
}