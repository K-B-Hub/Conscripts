//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TimerManager.h"
#include "EnemyAIController.generated.h"

class UBehaviorTree;
class UBlackboardData;
class AEnemyBase;
class ACharacterBase;

//전투 합류 사유
UENUM(BlueprintType)
enum class EJoinCombatReason : uint8
{
	Detection	UMETA(DisplayName = "감지"),
	Alarm		UMETA(DisplayName = "알람"),
	Proximity	UMETA(DisplayName = "근접 합류"),
	Attacked	UMETA(DisplayName = "피격"),
};

UCLASS()
class PW_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	//EnemyBase::InitTurn에서 호출
	void OnEnemyTurnStart();

	//턴 종료 처리
	void OnEnemyTurnEnd();

	bool IsInCombat() const { return bIsInCombat; }

	//전투 합류 처리, 실시간 비전투 BT 중단 — 턴은 다음 라운드 BuildTurnOrder에서 배정
	//AlarmOrigin은 Alarm 사유에만 사용, 교전 게이트 통과 전까지 접근 목표
	void JoinCombat(EJoinCombatReason Reason, const FVector& AlarmOrigin = FVector::ZeroVector);

	//전투 중인 캐릭터(플레이어 진영은 항상 전투중 판정) 감지 시 전투로 전환, 비전투 BT의 감지 태스크에서 호출
	void EvaluateDetectionAndMaybeJoinCombat();

	//전투 대상(플레이어 + 교전 중 적) 수집, 플레이어 진영은 항상 전투중 판정
	void CollectCombatants(TArray<ACharacterBase*>& OutCombatants) const;

	//부채꼴+시야로 감지되는 전투 대상 반환, 없으면 nullptr
	ACharacterBase* FindVisibleCombatant() const;

	//반경 내 전투 대상 존재 여부, 시야 무관 거리 판정
	bool HasNearbyCombatant(float Radius) const;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	//비전투 BT
	UPROPERTY(EditDefaultsOnly, Category = "AI|BehaviorTree")
	TObjectPtr<UBehaviorTree> nonCombatBT;

	//비전투 BT 블랙보드 데이터
	UPROPERTY(EditDefaultsOnly, Category = "AI|BehaviorTree")
	TObjectPtr<UBlackboardData> blackboardData;

	//전투 진입 여부
	bool bIsInCombat = false;

	//교전 게이트 반경, 알람 발생 지점 기준
	UPROPERTY(EditDefaultsOnly, Category = "AI|Combat")
	float engagementGateRadius = 1000.f;

	//알람 접근 중 근접 교전 전환 반경, 이 안에 전투 대상이 있으면 시야 무관 즉시 교전
	UPROPERTY(EditDefaultsOnly, Category = "AI|Combat")
	float engagementDetectRadius = 800.f;

	//턴 시작 후 첫 행동까지의 연출 딜레이
	UPROPERTY(EditDefaultsOnly, Category = "AI|Pacing")
	float turnStartDelay = 0.8f;

	//마지막 행동 후 다음 턴 진행까지의 연출 딜레이
	UPROPERTY(EditDefaultsOnly, Category = "AI|Pacing")
	float turnEndDelay = 0.8f;

	//시야 밖 턴 이동 배속
	UPROPERTY(EditDefaultsOnly, Category = "AI|Pacing")
	float fastForwardSpeedMultiplier = 3.f;

	//시야 밖 고속 턴 여부, 턴 시작 시 판정
	bool bFastForwardTurn = false;

	//배속 복원용 원래 이동 속도
	float normalWalkSpeed = 0.f;

	//알람 발생 지점, 게이트 통과 전까지 접근 목표
	FVector alarmFocusLocation = FVector::ZeroVector;
	bool bHasAlarmFocus = false;

	//블랙보드 키 이름
	static const FName BBKey_TargetActor;
	static const FName BBKey_InCombat;

private:
	//파동 반경 내 같은 진영을 전투로 전환, 파동·알람 합류자는 재전파하지 않음
	void BroadcastProximityWave();

	//비전투 실시간 BT 실행, BT 자체가 감지→순찰→대기를 반복
	void RunCurrentBT();

	//UtilityAI 턴 완료 콜백
	void OnUtilityAITurnComplete();

	//턴 시작 딜레이 후 실제 행동 개시, 게이트 검사와 UtilityAI 실행
	void BeginTurnAction();

	//턴 시작/종료 연출 딜레이 타이머
	FTimerHandle turnPacingTimerHandle;

	//알람 지점 접근 이동 시작, 이동 접수 시 true
	bool TryStartApproachToAlarm();

	//접근 이동 목적지 계산
	FVector ComputeApproachDestination(const FVector& PawnLoc, float BudgetCm) const;

	//접근 이동 완료 콜백
	UFUNCTION()
	void OnApproachMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
