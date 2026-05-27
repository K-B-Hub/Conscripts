// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TurnEnd.generated.h"

// 적 BT의 마지막 노드 — AIController::OnEnemyTurnEnd 호출로 GameMode에 턴 진행 위임
UCLASS()
class PW_API UBTTask_TurnEnd : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_TurnEnd();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};