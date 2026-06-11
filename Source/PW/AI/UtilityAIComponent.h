// Fill out your copyright notice in the Description page of Project Settings.

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

// 적 캐릭터에 부착되어 전투 턴 행동을 결정. BT 없이 자체 루프(ExecuteTurn)로 처리.
// 본 단계(§3)에서는 후보 열거 핵심만 구현. ExecuteTurn 루프·종합 EnumerateActions·옵션 B 토글은 §4에서.
UCLASS(ClassGroup=(AI), meta=(BlueprintSpawnableComponent))
class PW_API UUtilityAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUtilityAIComponent();
	
	virtual void BeginPlay() override;

protected:
	// 한 (스킬, 대상) 페어 + 이동 가정 여부에 대한 공격 후보 열거.
	//   bAssumeMoved=false: 캐스터 현재 위치에서만 시전 가능 여부 검사 (제자리 후보)
	//   bAssumeMoved=true : 대상 중심 피보나치 디스크 샘플 (이동 후 후보)
	// 호출자(EnumerateActions)가 옵션 B 가상 토글 후 bAssumeMoved=true로 호출.
	void EnumerateAttackActionsForTarget(UActiveSkillBase* Skill, ACharacterBase* Target,
	                                     bool bAssumeMoved,
	                                     TArray<FAIAction>& OutCandidates) const;

	// 순수 이동 후보 — 캐스터 중심 동심원 링 다중 샘플.
	// 5개 반지름 비율(잔여 이동력의 10/30/50/70/100%) × 각 링당 moveDirections 방향 = N 후보.
	// 각 후보에 대해 NavMesh 도달성 검사 + IncomingDanger 계산 → ScoreAction이 최선 선택.
	void EnumerateMoveActions(TArray<FAIAction>& OutCandidates) const;

	// 대기 후보 한 개. 명시적 후보로 두며(점수 0 fallback 아님), 좋은 위치 유지 시 능동적 최선이 될 수 있음.
	void EnumerateWaitAction(TArray<FAIAction>& OutCandidates) const;

	// 종합 후보 열거 — 모든 스킬 × 모든 대상 + 이동 + 대기.
	//   bExcludeMove=true: 직전 행동이 이동이었으면 호출 (이동 후보 + 이동 후 공격 후보 제외)
	// 옵션 B: 이동 가정 후보를 평가할 때만 OnMoveStateChanged 1회 가상 토글, 그룹 평가 후 rollback.
	void EnumerateActions(bool bExcludeMove, TArray<FAIAction>& OutCandidates) const;

	// 자유위치 디스크 균등 샘플 (황금각 나선) — 평면(Z 고정) 샘플.
	static void FibonacciDiskSample(const FVector& Center, float RadiusCm, int32 N,
	                                TArray<FVector>& OutSamples);

	// 위험도 — AtLocation에 ownerCharacter가 위치할 때 플레이어 측 전체로부터 받을 기대 피해 합.
	// 각 플레이어의 BattleGameMode에 기록된 FPlayerThreatProfile(관측된 최대 위협) × 거리/사선 게이팅.
	// 비용 = (살아있는 플레이어 수) × LOS/거리 검사 1회씩. 스킬 순회 없음.
	// 호출자(Enumerate*)가 후보 위치마다 1회만 호출해 FAIAction.IncomingDangerExpected에 캐싱해야 함.
	float ComputeIncomingDangerAt(const FVector& AtLocation) const;

	// 한 (스킬, 대상) 당 위치 샘플 개수. 첫 구현은 작게 두고 §5 검증 시 조정.
	UPROPERTY(EditDefaultsOnly, Category = "AI|Sampling")
	int32 attackPositionSamples = 10;

	// 순수 이동 후보 — 한 링당 방향 수. 5링 × 이 값 = 총 후보 수.
	UPROPERTY(EditDefaultsOnly, Category = "AI|Sampling")
	int32 moveDirectionsPerRing = 10;

	// 캐싱 — BeginPlay에서 owner 캐스팅 1회
	UPROPERTY(Transient)
	TObjectPtr<ACharacterBase> ownerCharacter = nullptr;

public:
	// 적 턴 시작 시 EnemyAIController에서 호출. 콜백 체인으로 후보 열거·평가·실행을 반복.
	// 3중 방어선: ① 자원 미감소 즉시 종료 ② 직전이 이동이면 이동 후보 제외 ③ MaxActionsPerTurn hard cap
	void ExecuteTurn();

	// 턴 종료 신호 — EnemyAIController가 구독해 GameMode->OnTurnEnd로 위임
	DECLARE_MULTICAST_DELEGATE(FOnAITurnComplete);
	FOnAITurnComplete OnTurnComplete;

protected:
	// 방어선 ③ hard cap. 정상 동작에선 < 8이어야 함. 도달 시 버그 신호.
	UPROPERTY(EditDefaultsOnly, Category = "AI|Loop")
	int32 maxActionsPerTurn = 8;

	// 행동과 행동 사이 텀 (초). 연속 행동이 한 번에 처리되지 않고 시각적으로 끊겨 보이도록.
	UPROPERTY(EditDefaultsOnly, Category = "AI|Loop")
	float actionDelay = 0.4f;

	// 후보 전체를 평가해 최고점 반환. 동점 시 먼저 등록된 후보.
	// OutScores 비-null이면 후보별 점수를 동일 인덱스로 채움 — 디버그 시각화가 ScoreAction을 재호출하지 않도록 (noise 일관성).
	const FAIAction* ScoreAndPickBest(const TArray<FAIAction>& Candidates, TArray<float>* OutScores = nullptr) const;

	// 디버그 시각화 — ai.UtilityDebug 1 토글 시 후보 위치·점수·선택지를 월드에 표시.
	void DrawDebugCandidates(const TArray<FAIAction>& Candidates,
	                         const TArray<float>& Scores,
	                         const FAIAction* Best) const;

	// 한 후보의 점수 — personalityData 가중치를 산식에 합산. 인라인 산식(첫 구현).
	// IncomingDanger(위험도 축)·이니셔티브 기반 정밀화는 설계 §3 후속 정교화 영역.
	float ScoreAction(const FAIAction& Action) const;

	// 적별 성향 — BP 인스턴스에서 디자이너가 할당. nullptr이면 CDO 기본값(Normal 성향) 사용.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Personality")
	TObjectPtr<UAIPersonalityData> personalityData;

	// 한 step — 후보 열거 → 점수 → 행동 디스패치. 비동기 이동의 경우 콜백 대기로 step 종료.
	void StepNext();

	// 턴 종료 — 콜백 해제 + OnTurnComplete 브로드캐스트
	void FinishTurn();

	// 제자리 공격/지원 즉시 실행 (회전 + SkillComponent::DirectExecute)
	void ExecuteAttackImmediate(const FAIAction& Action);

	// AIController::MoveTo 비동기 발사 — 이동력 사전 차감 + isMoved 전이 + ReceiveMoveCompleted 바인딩
	void StartMoveTo(const FVector& Dest, float PathLengthCm);

	// 이동 완료 콜백 — pending 공격 처리 후 다음 step
	UFUNCTION()
	void OnAIMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	// 방어선 ① + 자원 소진 검사 — true면 즉시 FinishTurn
	bool ShouldStopAfterStep() const;

	// 콜백 체인 상태
	int32 currentIter = 0;
	bool lastWasMove = false;
	int32 apBefore = 0;
	float moveBefore = 0.f;
	bool bHasPendingAfterMove = false;
	FAIAction pendingActionAfterMove;

	// 행동 사이 텀 타이머 핸들
	FTimerHandle stepTimerHandle;
};