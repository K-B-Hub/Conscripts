//Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask/BTTask_Patrol.h"
#include "Characters/EnemyBase.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UBTTask_Patrol::UBTTask_Patrol()
{
	NodeName = TEXT("Patrol");
	//실행별 상태 보존
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* aic = OwnerComp.GetAIOwner();
	if (!aic) return EBTNodeResult::Failed;

	AEnemyBase* enemy = Cast<AEnemyBase>(aic->GetPawn());
	if (!enemy) return EBTNodeResult::Failed;

	//순찰 불가 시 잠시 대기
	if (!enemy->CanPatrol())
	{
		UWorld* world = OwnerComp.GetWorld();
		if (!world) return EBTNodeResult::Succeeded;

		cachedOwnerComp = &OwnerComp;
		world->GetTimerManager().SetTimer(waitTimerHandle, this, &UBTTask_Patrol::OnWaitFinished, idleWaitTime, false);
		return EBTNodeResult::InProgress;
	}

	//비전투 적은 NavMesh 미등록 상태라 순찰 중 장애물 처리 불필요
	FAIMoveRequest req(enemy->GetCurrentPatrolWorldLocation());
	req.SetUsePathfinding(true);
	req.SetAcceptanceRadius(10.f);

	const EPathFollowingRequestResult::Type r = aic->MoveTo(req);
	if (r == EPathFollowingRequestResult::RequestSuccessful)
	{
		//이동 완료 콜백 대기
		cachedOwnerComp = &OwnerComp;
		aic->ReceiveMoveCompleted.AddUniqueDynamic(this, &UBTTask_Patrol::OnPatrolMoveCompleted);
		return EBTNodeResult::InProgress;
	}

	//이미 도착이면 인덱스 진행
	if (r == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		enemy->AdvancePatrolIndex();
	}
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTask_Patrol::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CleanupLatentState();
	return EBTNodeResult::Aborted;
}

void UBTTask_Patrol::CleanupLatentState()
{
	if (!IsValid(cachedOwnerComp)) return;

	//대기 타이머 정리
	if (UWorld* world = cachedOwnerComp->GetWorld())
	{
		world->GetTimerManager().ClearTimer(waitTimerHandle);
	}

	//완료 콜백 해제
	if (AAIController* aic = cachedOwnerComp->GetAIOwner())
	{
		aic->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_Patrol::OnPatrolMoveCompleted);
	}

	cachedOwnerComp = nullptr;
}

void UBTTask_Patrol::OnPatrolMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (!IsValid(cachedOwnerComp)) return;

	//도달 성공 시 다음 지점으로 진행
	if (AAIController* aic = cachedOwnerComp->GetAIOwner())
	{
		//완료 콜백 해제
		aic->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_Patrol::OnPatrolMoveCompleted);

		if (Result == EPathFollowingResult::Success)
		{
			if (AEnemyBase* enemy = Cast<AEnemyBase>(aic->GetPawn()))
			{
				enemy->AdvancePatrolIndex();
			}
		}
	}

	//도착 후 잠시 대기
	if (UWorld* world = cachedOwnerComp->GetWorld())
	{
		world->GetTimerManager().SetTimer(waitTimerHandle, this, &UBTTask_Patrol::OnWaitFinished, arriveWaitTime, false);
		return;
	}

	//월드가 없으면 즉시 종료
	UBehaviorTreeComponent* owner = cachedOwnerComp;
	CleanupLatentState();
	FinishLatentTask(*owner, EBTNodeResult::Succeeded);
}

void UBTTask_Patrol::OnWaitFinished()
{
	if (!IsValid(cachedOwnerComp)) return;

	UBehaviorTreeComponent* owner = cachedOwnerComp;
	CleanupLatentState();
	FinishLatentTask(*owner, EBTNodeResult::Succeeded);
}
