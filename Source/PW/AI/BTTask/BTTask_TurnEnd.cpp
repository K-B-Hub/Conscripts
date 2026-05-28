// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask/BTTask_TurnEnd.h"
#include "AI/AIController/EnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTTask_TurnEnd::UBTTask_TurnEnd()
{
	NodeName = TEXT("Turn End");
}

EBTNodeResult::Type UBTTask_TurnEnd::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyAIController* aic = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (!aic) return EBTNodeResult::Failed;

	aic->OnEnemyTurnEnd();
	return EBTNodeResult::Succeeded;
}