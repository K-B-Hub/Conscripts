//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAsset/MissionData.h"
#include "MissionData_Annihilate.generated.h"

//적 전멸 목표, 전장에 살아 있는 적이 없으면 달성
UCLASS(BlueprintType)
class PW_API UMissionData_Annihilate : public UMissionData
{
	GENERATED_BODY()

public:
	virtual bool IsComplete(const ABattleGameMode* battle) const override;
};