// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Enum/GameDifficulty.h"
#include "PWGameInstance.generated.h"

class UStressPoolData;
class UUpgradeTableData;
class UStoryRouteData;
class URunProgress;
class UPWSaveGame;

//레벨 전환 간 유지되는 런 상태 보관(난이도, 추후 캐릭터 상태·특전 등)
UCLASS()
class PW_API UPWGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	//런 진행 상태 생성 및 영구 진행도 로드
	virtual void Init() override;

	UFUNCTION(BlueprintPure, Category = "Game")
	EGameDifficulty GetDifficulty() const { return difficulty; }

	//메인메뉴에서 전투 시작 시 호출
	UFUNCTION(BlueprintCallable, Category = "Game")
	void SetDifficulty(EGameDifficulty newDifficulty) { difficulty = newDifficulty; }

	//스트레스 이벤트 풀 조회, 미설정 시 nullptr
	UStressPoolData* GetStressPool() const { return stressPool; }

	//공용 강화 후보 풀 조회, 미설정 시 nullptr
	UUpgradeTableData* GetCommonUpgradeTable() const { return commonUpgradeTable; }

	//스토리 줄기 목록, 줄기 선택 화면이 사용
	const TArray<TObjectPtr<UStoryRouteData>>& GetStoryRoutes() const { return storyRoutes; }

	//현재 런의 진행 상태, Init 이후 항상 유효
	URunProgress* GetRunProgress() const { return runProgress; }

	//영구 진행도, Init 이후 항상 유효(세이브가 없으면 새로 생성됨)
	UPWSaveGame* GetSaveGame() const { return saveGame; }

	//영구 진행도를 슬롯에 기록
	void SaveProgress();

	//편성 확정 시 호출, 이전 런의 흔적이 남지 않도록 진행 상태를 새로 만든다
	//스토리가 아니면 storyRouteId는 NAME_None
	void StartRun(FName storyRouteId);

protected:
	//현재 런의 난이도
	//메인메뉴를 거치지 않고 전투 맵에 직접 들어가 테스트할 때의 기본값 역할도 한다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	EGameDifficulty difficulty = EGameDifficulty::Stage;

	//스트레스 한계 도달 시 사용할 이벤트 풀, 런 전체에서 공유
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stress")
	TObjectPtr<UStressPoolData> stressPool;

	//모든 직업이 공통으로 얻을 수 있는 강화 후보 풀, 런 전체에서 공유
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	TObjectPtr<UUpgradeTableData> commonUpgradeTable;

	//스토리 모드가 제공하는 줄기 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Story")
	TArray<TObjectPtr<UStoryRouteData>> storyRoutes;

private:
	//런 진행 상태, 런 시작 시 통째로 교체
	UPROPERTY()
	TObjectPtr<URunProgress> runProgress;

	//영구 진행도
	UPROPERTY()
	TObjectPtr<UPWSaveGame> saveGame;
};