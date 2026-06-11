// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "TimerManager.h"
#include "BTTask_Patrol.generated.h"

// 비전투 적의 순찰 Task — EnemyBase의 순찰 지점(스폰 기준 상대좌표)으로 순서대로 이동.
// MoveTo가 끝날 때까지 latent(InProgress), 도달하면 인덱스 진행 후 Succeeded.
UCLASS()
class PW_API UBTTask_Patrol : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Patrol();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//BT 중단 시 정리 — 델리게이트/타이머 콜백 누수 방지
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	//델리게이트 언바인드 + 타이머 clear + 캐시 무효화 — 정상완료/Abort 공용
	void CleanupLatentState();

	//MoveTo 완료 콜백 — 인덱스 진행 후 latent task 종료
	UFUNCTION()
	void OnPatrolMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	//대기 종료 — latent task 완료 (idle 대기 / 도달 후 종료 텀 공용)
	void OnWaitFinished();

	//순찰 지점이 없을 때 대기 시간(초) — 적이 가만히 있음을 보여주는 연출
	UPROPERTY(EditAnywhere, Category = "Patrol", meta = (AllowPrivateAccess = "true"))
	float idleWaitTime = 0.8f;

	//순찰 지점 도달 후 종료까지 대기 시간(초) — 턴이 너무 빨리 넘어가지 않게 가시성 확보
	UPROPERTY(EditAnywhere, Category = "Patrol", meta = (AllowPrivateAccess = "true"))
	float arriveWaitTime = 0.5f;

	//대기 타이머 핸들 (idle/도달 후 공용)
	FTimerHandle waitTimerHandle;

	//latent 종료를 위해 캐시 — bCreateNodeInstance=true라 인스턴스마다 독립
	TObjectPtr<UBehaviorTreeComponent> cachedOwnerComp;
};