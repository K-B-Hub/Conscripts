//Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService/BTService_DetectAndJoinCombat.h"
#include "AI/AIController/EnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTService_DetectAndJoinCombat::UBTService_DetectAndJoinCombat()
{
	NodeName = TEXT("Detect And Join Combat");
	Interval = 0.3f;
	RandomDeviation = 0.1f;
}

void UBTService_DetectAndJoinCombat::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AEnemyAIController* aic = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (aic && !aic->IsInCombat())
	{
		aic->EvaluateDetectionAndMaybeJoinCombat();
	}
}
