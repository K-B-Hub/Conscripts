//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DetectAndJoinCombat.generated.h"

//비전투 BT 첫 노드용 감지 Task, 전투 중인 캐릭터 감지 시 전투로 전환
//전환 시 JoinCombat이 BT를 Safe 정지시키므로 이후 노드는 실행되지 않음
UCLASS()
class PW_API UBTTask_DetectAndJoinCombat : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_DetectAndJoinCombat();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
