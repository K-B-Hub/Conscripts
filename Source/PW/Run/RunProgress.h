//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Run/AllyRunState.h"
#include "Run/StageEntry.h"
#include "RunProgress.generated.h"

class AAllyCharacterBase;
class URunModeData;

//한 번의 런 동안 유지되는 진행 상태
//레벨 전환에도 살아남아야 하므로 GameInstance가 소유하고, 런 시작 시 통째로 새로 만든다
UCLASS()
class PW_API URunProgress : public UObject
{
	GENERATED_BODY()

public:
	//미리 짜인 시퀀스로 런을 연다, 스토리 모드
	void StartFixedRun(const TArray<FAllyRunState>& initialRoster, FName inStoryRouteId, const TArray<FStageEntry>& inStages);

	//규칙으로 시퀀스를 만들어 가며 런을 연다, 로그라이크·악몽
	//첫 스테이지는 언제나 전투다
	void StartGeneratedRun(const TArray<FAllyRunState>& initialRoster, URunModeData* inModeData);

	//런 종료, 메인메뉴 복귀 시 호출
	void EndRun();

	//현재 스테이지 번호, 0부터 시작
	int32 GetStageIndex() const { return stageIndex; }

	//현재 진행할 스테이지, 시퀀스를 벗어났으면 nullptr
	const FStageEntry* GetCurrentStage() const;

	//현재 기준 offset칸 뒤의 스테이지, 없으면 nullptr
	//예고 화면이 다음과 그 다음을 함께 보여주므로 앞을 내다볼 수 있어야 한다
	const FStageEntry* GetUpcomingStage(int32 offset) const;

	//아직 진행할 스테이지가 남아 있는지
	bool HasMoreStages() const { return stages.IsValidIndex(stageIndex); }

	//현재 스테이지를 마치고 다음으로, 런이 끝나면 false
	//전투를 마친 경우에만 전투 수가 오르고, 생성 모드면 다음 스테이지를 여기서 뽑는다
	bool AdvanceStage();

	//다음 스테이지 앞에 야영지를 끼워 넣는다, 남은 횟수가 없으면 false
	//악몽의 자유 야영지, 예고 화면에서 호출한다
	bool InsertCampVisit();

	//예고 화면 표시용
	int32 GetBattlesCleared() const { return battlesCleared; }
	int32 GetTotalBattles() const;
	int32 GetCampVisitsLeft() const { return campVisitsLeft; }

	bool IsRunActive() const { return bRunActive; }

	//스토리 모드에서 진행 중인 줄기, 다른 모드에서는 NAME_None
	//런이 열릴 때 확정되므로 별도 setter를 두지 않는다
	FName GetStoryRouteId() const { return storyRouteId; }

	const TArray<FAllyRunState>& GetRoster() const { return roster; }

	//중간 영입, 야영지·이벤트에서 새 캐릭터가 합류할 때 호출
	void AddToRoster(const FAllyRunState& newMember);

	//스테이지 진입 시 로스터를 스폰된 캐릭터들에게 배분
	//전원 출격이므로 배열 순서대로 1:1 대응한다
	void RestoreToWorld(const TArray<AAllyCharacterBase*>& allies) const;

	//스테이지 종료 시 결과 회수, 살아 돌아온 캐릭터만 로스터에 남는다
	void CaptureFromWorld(const TArray<AAllyCharacterBase*>& allies);

private:
	//예고에 필요한 만큼 시퀀스를 미리 뽑아 채운다, 목표 전투 수를 넘겨 만들지는 않는다
	void EnsureLookahead();

	//아직 치르지 않은 전투 수, 예고에 잡힌 것까지 센다
	int32 CountPendingBattles() const;

	//다음 스테이지를 뽑는다, bAfterBattle이면 야영지 추첨을 거친다
	//야영지 다음은 언제나 전투라 야영지가 연달아 나오지 않는다
	FStageEntry RollNextStage(bool bAfterBattle);

	//전투 후보 하나, 풀이 비면 빈 항목이라 이동이 실패하고 런이 닫힌다
	FStageEntry PickFromPool(const TArray<FStageEntry>& pool) const;

	//현재 런의 아군 구성, 사망 시 줄고 영입 시 늘어난다
	UPROPERTY()
	TArray<FAllyRunState> roster;

	//지나온 스테이지와 예고된 다음 스테이지, 생성 모드에서는 진행하며 늘어난다
	UPROPERTY()
	TArray<FStageEntry> stages;

	//생성 규칙, 스토리처럼 시퀀스가 고정된 런에서는 nullptr
	UPROPERTY()
	TObjectPtr<URunModeData> modeData = nullptr;

	//마친 전투 수, 생성 모드의 완주 판정 기준. 야영지는 세지 않는다
	int32 battlesCleared = 0;

	//야영지가 연속으로 나오지 않은 횟수, 등장 확률을 끌어올린다
	int32 campSkipStreak = 0;

	//플레이어가 아직 쓸 수 있는 야영지 방문 횟수
	int32 campVisitsLeft = 0;

	//진행 중인 스테이지 번호
	int32 stageIndex = 0;

	//런 진행 여부
	bool bRunActive = false;

	//스토리 줄기 식별자, 클리어 시 세이브에 기록할 대상
	FName storyRouteId = NAME_None;
};