//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_DetectAndJoinCombat.generated.h"

//비전투 BT 전 구간에서 주기적으로 감지 검사, 순찰 이동 중에도 전투 전환
UCLASS()
class PW_API UBTService_DetectAndJoinCombat : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_DetectAndJoinCombat();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
