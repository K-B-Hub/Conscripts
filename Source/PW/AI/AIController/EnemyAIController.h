// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UBehaviorTree;
class UBlackboardData;
class AEnemyBase;

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

	// 현재 상태(bIsInCombat)에 맞는 BT 실행
	void RunCurrentBT();
};