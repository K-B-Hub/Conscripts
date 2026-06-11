// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask/BTTask_Patrol.h"
#include "Characters/EnemyBase.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UBTTask_Patrol::UBTTask_Patrol()
{
	NodeName = TEXT("Patrol");
	//인스턴스화 — cachedOwnerComp/델리게이트 바인딩을 실행마다 독립 보존
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* aic = OwnerComp.GetAIOwner();
	if (!aic) return EBTNodeResult::Failed;

	AEnemyBase* enemy = Cast<AEnemyBase>(aic->GetPawn());
	if (!enemy) return EBTNodeResult::Failed;

	//왕복할 지점이 없으면(원위치 1개뿐) idleWaitTime 동안 대기 후 종료 — 적이 가만히 있음을 보여주는 연출.
	//즉시 완료하지 않고 타이머로 미뤄 카메라가 적에게 잠시 머물게 함
	if (!enemy->CanPatrol())
	{
		UWorld* world = OwnerComp.GetWorld();
		if (!world) return EBTNodeResult::Succeeded;

		cachedOwnerComp = &OwnerComp;
		world->GetTimerManager().SetTimer(idleTimerHandle, this, &UBTTask_Patrol::OnIdleWaitFinished, idleWaitTime, false);
		return EBTNodeResult::InProgress;
	}

	FAIMoveRequest req(enemy->GetCurrentPatrolWorldLocation());
	req.SetUsePathfinding(true);
	req.SetAcceptanceRadius(10.f);

	const EPathFollowingRequestResult::Type r = aic->MoveTo(req);
	if (r == EPathFollowingRequestResult::RequestSuccessful)
	{
		//이동 진행 중 — 완료 콜백 대기 (latent)
		cachedOwnerComp = &OwnerComp;
		aic->ReceiveMoveCompleted.AddUniqueDynamic(this, &UBTTask_Patrol::OnPatrolMoveCompleted);
		return EBTNodeResult::InProgress;
	}

	//즉시 처리 — 이미 도착이면 인덱스 진행, 실패면 그대로 통과시켜 턴 종료 보장
	if (r == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		enemy->AdvancePatrolIndex();
	}
	return EBTNodeResult::Succeeded;
}

void UBTTask_Patrol::OnPatrolMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (!cachedOwnerComp) return;

	//콜백 해제 — 다음 실행/다른 적의 콜백 누수 방지
	if (AAIController* aic = cachedOwnerComp->GetAIOwner())
	{
		aic->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_Patrol::OnPatrolMoveCompleted);

		//도달했으니 다음 순찰 지점으로 진행
		if (AEnemyBase* enemy = Cast<AEnemyBase>(aic->GetPawn()))
		{
			enemy->AdvancePatrolIndex();
		}
	}

	UBehaviorTreeComponent* owner = cachedOwnerComp;
	cachedOwnerComp = nullptr;
	FinishLatentTask(*owner, EBTNodeResult::Succeeded);
}

void UBTTask_Patrol::OnIdleWaitFinished()
{
	if (!cachedOwnerComp) return;

	UBehaviorTreeComponent* owner = cachedOwnerComp;
	cachedOwnerComp = nullptr;
	FinishLatentTask(*owner, EBTNodeResult::Succeeded);
}