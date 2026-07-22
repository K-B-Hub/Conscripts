//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StressPoolData.generated.h"

//스트레스 한계 도달 시 적용할 이벤트 풀, 버프/상태이상/패시브 클래스를 계열 구분 없이 담고 적용 시점에 분기
UCLASS(BlueprintType)
class PW_API UStressPoolData : public UDataAsset
{
	GENERATED_BODY()

public:
	//긍정 이벤트 풀(20%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stress")
	TArray<TSubclassOf<UObject>> positiveEvents;

	//부정 이벤트 풀(80%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stress")
	TArray<TSubclassOf<UObject>> negativeEvents;
};
