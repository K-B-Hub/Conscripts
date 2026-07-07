//Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask/BTTask_DetectAndJoinCombat.h"
#include "AI/AIController/EnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTTask_DetectAndJoinCombat::UBTTask_DetectAndJoinCombat()
{
	NodeName = TEXT("Detect And Join Combat");
}

EBTNodeResult::Type UBTTask_DetectAndJoinCombat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyAIController* aic = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (!aic) return EBTNodeResult::Failed;

	if (!aic->IsInCombat())
	{
		aic->EvaluateDetectionAndMaybeJoinCombat();
	}
	return EBTNodeResult::Succeeded;
}
