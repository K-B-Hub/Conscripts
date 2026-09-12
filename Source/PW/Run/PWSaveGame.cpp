//Fill out your copyright notice in the Description page of Project Settings.

#include "Run/PWSaveGame.h"

bool UPWSaveGame::IsModeUnlocked(EGameDifficulty mode) const
{
	//스토리는 진입점이므로 항상 개방
	if (mode == EGameDifficulty::Stage) return true;

	//로그라이크·악몽은 스토리 줄기를 하나라도 클리어하면 함께 열림
	return clearedStoryRoutes.Num() > 0;
}

void UPWSaveGame::RegisterStoryClear(FName routeId)
{
	if (routeId.IsNone()) return;

	clearedStoryRoutes.AddUnique(routeId);
}