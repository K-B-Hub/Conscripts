//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "TimerManager.h"
#include "BTTask_Patrol.generated.h"

//비전투 적 순찰 Task
UCLASS()
class PW_API UBTTask_Patrol : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Patrol();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//BT 중단 시 정리
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	//비동기 상태 정리
	void CleanupLatentState();

	//MoveTo 완료 콜백
	UFUNCTION()
	void OnPatrolMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	//대기 종료 콜백
	void OnWaitFinished();

	//순찰 지점 없을 때 대기 시간
	UPROPERTY(EditAnywhere, Category = "Patrol", meta = (AllowPrivateAccess = "true"))
	float idleWaitTime = 0.8f;

	//순찰 지점 도달 후 대기 시간
	UPROPERTY(EditAnywhere, Category = "Patrol", meta = (AllowPrivateAccess = "true"))
	float arriveWaitTime = 0.5f;

	//대기 타이머 핸들
	FTimerHandle waitTimerHandle;

	//BT 컴포넌트 캐시
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> cachedOwnerComp;
};
