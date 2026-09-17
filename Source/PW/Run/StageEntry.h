//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StageEntry.generated.h"

class UMissionData;

UENUM(BlueprintType)
enum class EStageType : uint8
{
	Battle	UMETA(DisplayName = "Battle"),	//전투 스테이지
	Camp	UMETA(DisplayName = "Camp")		//야영지
};

//런의 스테이지 하나
//시퀀스를 어디서 만드는지(줄기 고정·모드별·랜덤 생성)는 런 시작 시점의 호출자가 정한다
USTRUCT(BlueprintType)
struct FStageEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EStageType Type = EStageType::Battle;

	//이 스테이지에서 열 레벨
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> Map;

	//승리 조건, 맵과 분리해 두어 같은 맵을 여러 임무로 재사용한다
	//야영지 스테이지에서는 비워 둔다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMissionData> Mission;
};