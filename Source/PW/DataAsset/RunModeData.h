//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Run/StageEntry.h"
#include "RunModeData.generated.h"

//시퀀스를 미리 정해두지 않고 규칙으로 만들어 가는 모드의 설정
//로그라이크와 악몽은 분기가 아니라 이 값들의 차이로만 구분된다
//악몽은 campChanceBase를 0으로 두어 자동 출현을 막고 playerCampVisits만 연다
UCLASS(BlueprintType)
class PW_API URunModeData : public UDataAsset
{
	GENERATED_BODY()

public:
	//런이 끝나는 전투 수, 야영지는 세지 않는다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Run")
	int32 totalBattles = 12;

	//전투 스테이지 후보, 맵과 임무가 짝지어진 채로 하나를 뽑는다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Run")
	TArray<FStageEntry> battlePool;

	//야영지 후보
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camp")
	TArray<FStageEntry> campPool;

	//전투를 마쳤을 때 야영지가 나올 기본 확률
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camp")
	float campChanceBase = 0.3f;

	//야영지가 나오지 않을 때마다 더해지는 확률, 나오면 다시 기본값으로 돌아간다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camp")
	float campChanceStep = 0.1f;

	//플레이어가 원하는 시점에 야영지를 끼워 넣을 수 있는 횟수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camp")
	int32 playerCampVisits = 0;
};