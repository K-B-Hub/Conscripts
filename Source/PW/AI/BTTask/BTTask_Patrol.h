// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Navigation/PathFollowingComponent.h"
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

private:
	//MoveTo 완료 콜백 — 인덱스 진행 후 latent task 종료
	UFUNCTION()
	void OnPatrolMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	//순찰할 곳이 없을 때 대기 종료 — latent task 완료
	void OnIdleWaitFinished();

	//순찰 지점이 없을 때 대기 시간(초) — 적이 가만히 있음을 보여주는 연출
	UPROPERTY(EditAnywhere, Category = "Patrol", meta = (AllowPrivateAccess = "true"))
	float idleWaitTime = 0.8f;

	//대기 타이머 핸들
	FTimerHandle idleTimerHandle;

	//latent 종료를 위해 캐시 — bCreateNodeInstance=true라 인스턴스마다 독립
	TObjectPtr<UBehaviorTreeComponent> cachedOwnerComp;
};