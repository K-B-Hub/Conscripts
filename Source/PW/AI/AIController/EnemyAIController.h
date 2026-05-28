// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UBehaviorTree;
class UBlackboardData;
class AEnemyBase;

// 전투 합류 사유 — 모든 합류는 JoinCombat 한 함수를 통과한다 (설계 §2 단일 진입점)
UENUM(BlueprintType)
enum class EJoinCombatReason : uint8
{
	Detection	UMETA(DisplayName = "감지"),
	Alarm		UMETA(DisplayName = "알람"),
	Proximity	UMETA(DisplayName = "근접 합류"),
};

UCLASS()
class PW_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	// EnemyBase::InitTurn에서 호출 — 비전투면 감지 평가 후 필요 시 전투 BT로 스왑, 그 다음 현재 BT 진행
	void OnEnemyTurnStart();

	// 턴 종료 — BT 측 FinishTurn Task에서 호출하여 GameMode에 다음 턴 진행을 위임
	void OnEnemyTurnEnd();

	bool IsInCombat() const { return bIsInCombat; }

	// 전투 합류 단일 진입점 — 감지/알람/근접 모두 이 함수를 호출. 이미 전투면 no-op.
	// 첫 구현: 플래그 토글 + 블랙보드 갱신 + 로그. 추후 GameMode 전투 그룹 등록·이니셔티브 삽입이 여기로 들어옴.
	void JoinCombat(EJoinCombatReason Reason);

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// 비전투 BT — 패트롤/대기 등
	UPROPERTY(EditDefaultsOnly, Category = "AI|BehaviorTree")
	TObjectPtr<UBehaviorTree> nonCombatBT;

	// 전투 BT — 교전 행동
	UPROPERTY(EditDefaultsOnly, Category = "AI|BehaviorTree")
	TObjectPtr<UBehaviorTree> combatBT;

	// 두 BT가 공유하는 블랙보드 데이터 (TargetActor 키 등을 정의)
	UPROPERTY(EditDefaultsOnly, Category = "AI|BehaviorTree")
	TObjectPtr<UBlackboardData> blackboardData;

	// 한번 true가 되면 다시 false로 돌아오지 않음 — 설계 결정 (비전투 → 전투 단방향)
	bool bIsInCombat = false;

	// 블랙보드 키 이름 — BT 에셋의 키 이름과 일치해야 함
	static const FName BBKey_TargetActor;
	static const FName BBKey_InCombat;

private:
	// 비전투 상태일 때 시야 내 아군 탐색 — 발견 시 전투 상태로 전환하고 BT 스왑
	void EvaluateDetectionAndMaybeSwitch();

	// 비전투 BT 실행 (전투는 UtilityAI가 담당)
	void RunCurrentBT();

	// UtilityAI 턴 완료 콜백 — OnEnemyTurnEnd 위임
	void OnUtilityAITurnComplete();
};