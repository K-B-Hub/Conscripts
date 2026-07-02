//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "AI/AIAction.h"
#include "TimerManager.h"
#include "UtilityAIComponent.generated.h"

class UActiveSkillBase;
class ACharacterBase;
class UAIPersonalityData;

//적 전투 턴 행동을 결정하는 컴포넌트
UCLASS(ClassGroup=(AI), meta=(BlueprintSpawnableComponent))
class PW_API UUtilityAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUtilityAIComponent();
	
	virtual void BeginPlay() override;

protected:
	//스킬과 대상 기준 공격 후보 열거
	void EnumerateAttackActionsForTarget(UActiveSkillBase* Skill, ACharacterBase* Target,
	                                     bool bAssumeMoved,
	                                     TArray<FAIAction>& OutCandidates) const;

	//순수 이동 후보 열거
	void EnumerateMoveActions(TArray<FAIAction>& OutCandidates) const;

	//대기 후보 추가
	void EnumerateWaitAction(TArray<FAIAction>& OutCandidates) const;

	//전체 행동 후보 열거
	void EnumerateActions(bool bExcludeMove, TArray<FAIAction>& OutCandidates) const;

	//디스크 위치 샘플 생성
	static void FibonacciDiskSample(const FVector& Center, float RadiusCm, int32 N,
	                                TArray<FVector>& OutSamples);

	//지정 위치의 예상 피격 피해 계산
	float ComputeIncomingDangerAt(const FVector& AtLocation) const;

	//공격 위치 샘플 수
	UPROPERTY(EditDefaultsOnly, Category = "AI|Sampling")
	int32 attackPositionSamples = 10;

	//이동 링당 방향 수
	UPROPERTY(EditDefaultsOnly, Category = "AI|Sampling")
	int32 moveDirectionsPerRing = 10;

	//소유 캐릭터 캐시
	UPROPERTY(Transient)
	TObjectPtr<ACharacterBase> ownerCharacter = nullptr;

public:
	//적 턴 행동 시작
	void ExecuteTurn();

	//턴 종료 신호
	DECLARE_MULTICAST_DELEGATE(FOnAITurnComplete);
	FOnAITurnComplete OnTurnComplete;

protected:
	//턴당 최대 행동 수
	UPROPERTY(EditDefaultsOnly, Category = "AI|Loop")
	int32 maxActionsPerTurn = 8;

	//행동 사이 대기 시간
	UPROPERTY(EditDefaultsOnly, Category = "AI|Loop")
	float actionDelay = 0.4f;

	//최고점 후보 선택
	const FAIAction* ScoreAndPickBest(const TArray<FAIAction>& Candidates, TArray<float>* OutScores = nullptr) const;

	//후보 디버그 시각화
	void DrawDebugCandidates(const TArray<FAIAction>& Candidates,
	                         const TArray<float>& Scores,
	                         const FAIAction* Best) const;

	//단일 후보 점수 계산
	float ScoreAction(const FAIAction& Action) const;

	//적별 성향 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Personality")
	TObjectPtr<UAIPersonalityData> personalityData;

	//다음 행동 단계 실행
	void StepNext();

	//턴 종료 처리
	void FinishTurn();

	//제자리 공격 실행
	void ExecuteAttackImmediate(const FAIAction& Action);

	//AI 이동 시작
	void StartMoveTo(const FVector& Dest, float PathLengthCm);

	//AI 이동 완료 콜백
	UFUNCTION()
	void OnAIMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	//행동 후 종료 여부 검사
	bool ShouldStopAfterStep() const;

	//콜백 체인 상태
	int32 currentIter = 0;
	bool lastWasMove = false;
	int32 apBefore = 0;
	float moveBefore = 0.f;
	bool bHasPendingAfterMove = false;
	FAIAction pendingActionAfterMove;

	//행동 대기 타이머
	FTimerHandle stepTimerHandle;
};
