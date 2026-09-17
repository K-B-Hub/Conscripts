//Fill out your copyright notice in the Description page of Project Settings.

#include "DataAsset/MissionData_Annihilate.h"
#include "GameMode/BattleGameMode.h"

bool UMissionData_Annihilate::IsComplete(const ABattleGameMode* battle) const
{
	if (!battle) return false;

	//사망 시 OnCharacterDeath가 배열에서 제거하므로 남은 원소가 곧 생존한 적이다
	return battle->GetEnemies().Num() == 0;
}