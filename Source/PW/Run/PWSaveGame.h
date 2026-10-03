//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Enum/GameDifficulty.h"
#include "Run/AllyRunState.h"
#include "Run/StageEntry.h"
#include "PWSaveGame.generated.h"

class URunModeData;

//진행 중인 런의 디스크 스냅샷, URunProgress의 저장 대상 필드와 1:1로 대응한다
//RunProgress에 런 상태 필드를 추가하면 여기와 ExportTo·RestoreFrom 세 곳을 함께 고쳐야 한다
USTRUCT()
struct FRunSaveState
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FAllyRunState> roster;

	UPROPERTY()
	TArray<FStageEntry> stages;

	UPROPERTY()
	TObjectPtr<URunModeData> modeData = nullptr;

	UPROPERTY()
	int32 battlesCleared = 0;

	UPROPERTY()
	int32 campSkipStreak = 0;

	UPROPERTY()
	int32 campVisitsLeft = 0;

	UPROPERTY()
	int32 stageIndex = 0;

	UPROPERTY()
	FName storyRouteId = NAME_None;

	//난이도는 D4에 따라 GameInstance가 소유하므로 RunProgress 밖에서 따로 담는다
	//적 인원과 경험치 계수를 정하므로 빠뜨리면 이어한 런의 규모가 달라진다
	UPROPERTY()
	EGameDifficulty difficulty = EGameDifficulty::Stage;

	//런이 닫히면 통째로 기본값으로 되돌아간다
	//별도 플래그를 두지 않는 이유는 둘이 어긋날 여지를 만들지 않기 위함 — 로스터가 비면 이어할 런이 없다
	bool IsValid() const { return roster.Num() > 0; }
};

//디스크에 영구 보존되는 세이브
//메타 진행도(클리어한 줄기)와 진행 중인 런을 함께 담는다
UCLASS()
class PW_API UPWSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	//클리어한 스토리 줄기, 모드 해금과 직업 해금의 단일 근거
	//해금 직업 목록을 직접 담지 않는 이유는 매핑 변경이 기존 세이브에 반영되게 하기 위함
	UPROPERTY()
	TArray<FName> clearedStoryRoutes;

	//진행 중인 런, 메인메뉴로 나가면 기록되고 런이 닫히거나 새 런이 열리면 비워진다
	UPROPERTY()
	FRunSaveState run;

	//해당 모드를 선택할 수 있는지, 스토리는 항상 개방이고 나머지는 스토리 1줄기 클리어 필요
	bool IsModeUnlocked(EGameDifficulty mode) const;

	//해당 줄기를 클리어했는지, 줄기 선택 화면의 표시용
	bool IsStoryRouteCleared(FName routeId) const { return clearedStoryRoutes.Contains(routeId); }

	//줄기 클리어 기록, 중복 클리어는 누적하지 않음
	void RegisterStoryClear(FName routeId);
};