// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Enum/GameDifficulty.h"
#include "Run/AllyRunState.h"
#include "Run/StageEntry.h"
#include "PWGameInstance.generated.h"

class UStressPoolData;
class UUpgradeTableData;
class UStoryRouteData;
class URunModeData;
class URunProgress;
class UPWSaveGame;
class AAllyCharacterBase;

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

	//식별자로 줄기 조회, 없으면 nullptr
	const UStoryRouteData* FindStoryRoute(FName routeId) const;

	//지금 고를 수 있는 직업, 기본 직업 + 클리어한 줄기가 여는 직업
	//편성과 야영지 충원이 같은 목록을 쓰므로 인덱스도 서로 맞는다
	TArray<TSubclassOf<AAllyCharacterBase>> GetUnlockedJobs() const;

	//로그라이크·악몽의 시작 편성 인원
	int32 GetInitialRosterSize() const { return initialRosterSize; }

	//로스터가 이 수를 넘지 못한다, 야영지 자리 수와 별개로 규칙으로 두는 상한
	int32 GetMaxRosterSize() const { return maxRosterSize; }

	//런 종료·패배 시 돌아갈 허브 레벨
	const TSoftObjectPtr<UWorld>& GetHubMap() const { return hubMap; }

	//현재 스테이지 맵을 연다, 시퀀스를 벗어났거나 맵이 없으면 false
	//편성 확정·전투 종료·야영지 퇴장이 모두 같은 판단을 하므로 시퀀스를 소유한 이쪽에 둔다
	bool TravelToCurrentStage(const UObject* worldContext);

	//런을 닫고 허브로 복귀, bCleared면 줄기 클리어를 기록한다
	//런이 닫히는 유일한 지점이라 클리어 기록도 여기 한 곳에서만 일어난다
	void EndRunAndReturnToHub(const UObject* worldContext, bool bCleared);

	//현재 런의 진행 상태, Init 이후 항상 유효
	URunProgress* GetRunProgress() const { return runProgress; }

	//영구 진행도, Init 이후 항상 유효(세이브가 없으면 새로 생성됨)
	UPWSaveGame* GetSaveGame() const { return saveGame; }

	//영구 진행도를 슬롯에 기록
	void SaveProgress();

	//편성 확정 시 호출, 이전 런의 흔적이 남지 않도록 진행 상태를 새로 만든다
	//시퀀스를 고정으로 줄지 규칙으로 만들지는 난이도를 아는 이쪽이 판단한다
	void StartRun(const TArray<FAllyRunState>& roster, FName storyRouteId);

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

	//해금과 무관하게 언제나 고를 수 있는 기본 직업
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Job")
	TArray<TSubclassOf<AAllyCharacterBase>> selectableJobs;

	//로그라이크·악몽의 시작 편성 인원, 밸런스 값이라 에디터에서 조정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Job")
	int32 initialRosterSize = 6;

	//로스터 상한, 야영지 자리가 모자라 못 세우는 것과 달리 이쪽은 의도된 규칙이다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Job")
	int32 maxRosterSize = 12;

	//줄기나 모드 설정이 비어 있을 때 쓰는 대체 시퀀스
	//콘텐츠가 채워지기 전까지 모든 모드가 이걸로 돌아간다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stage")
	TArray<FStageEntry> defaultStageSequence;

	//시퀀스를 규칙으로 만드는 모드의 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stage")
	TObjectPtr<URunModeData> roguelikeMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stage")
	TObjectPtr<URunModeData> nightmareMode;

	//런이 끝나면 돌아갈 허브 레벨
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stage")
	TSoftObjectPtr<UWorld> hubMap;

private:
	//런 진행 상태, 런 시작 시 통째로 교체
	UPROPERTY()
	TObjectPtr<URunProgress> runProgress;

	//영구 진행도
	UPROPERTY()
	TObjectPtr<UPWSaveGame> saveGame;
};