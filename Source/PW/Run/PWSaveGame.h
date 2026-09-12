//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Enum/GameDifficulty.h"
#include "PWSaveGame.generated.h"

//런과 무관하게 영구 보존되는 메타 진행도
//런 도중 상태는 저장하지 않음, 이어하기는 전체 흐름 완성 후 별도 도입
UCLASS()
class PW_API UPWSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	//클리어한 스토리 줄기, 모드 해금과 직업 해금의 단일 근거
	//해금 직업 목록을 직접 담지 않는 이유는 매핑 변경이 기존 세이브에 반영되게 하기 위함
	UPROPERTY()
	TArray<FName> clearedStoryRoutes;

	//해당 모드를 선택할 수 있는지, 스토리는 항상 개방이고 나머지는 스토리 1줄기 클리어 필요
	bool IsModeUnlocked(EGameDifficulty mode) const;

	//해당 줄기를 클리어했는지, 줄기 선택 화면의 표시용
	bool IsStoryRouteCleared(FName routeId) const { return clearedStoryRoutes.Contains(routeId); }

	//줄기 클리어 기록, 중복 클리어는 누적하지 않음
	void RegisterStoryClear(FName routeId);
};