//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AIPersonalityData.generated.h"

//적 AI 성향별 점수 가중치
UCLASS(BlueprintType)
class PW_API UAIPersonalityData : public UDataAsset
{
	GENERATED_BODY()

public:
	//성향 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	FName personalityName = TEXT("Normal");

	//기대 피해 가중치
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Threat")
	float weightExpectedDamage = 1.0f;

	//확정 처치 보너스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Threat")
	float weightCanKillBonus = 50.0f;

	//치명타 처치 보너스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Threat")
	float weightCanCritKillBonus = 15.0f;

	//명중률 보너스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Threat")
	float weightHitChance = 10.0f;

	//오버킬 페널티
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Threat")
	float weightOverkillPenalty = -0.5f;

	//아군 오사 피해 페널티, 범위/멀티픽이 같은 진영을 맞출 때 적용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Threat")
	float weightAllyDamagePenalty = -2.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Heal")
	float weightExpectedHeal = 0.8f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Heal")
	float weightHealTargetLowHp = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Heal")
	float weightHealTargetExtremeLowHp = 50.f;
	
	//대상 위협도 가중치
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Threat")
	float weightTargetThreat = 0.2f;

	//낮은 체력 대상 보너스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Threat")
	float weightTargetLowHp = 20.0f;

	//이동 거리 페널티
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Cost")
	float weightDistancePenalty = -0.001f;

	//대기 점수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Cost")
	float weightWait = -10.0f;

	//순수 이동 보너스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Cost")
	float weightMoveAdvance = 0.005f;

	//자원 비용 페널티
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Cost")
	float weightResourceCost = -1.0f;

	//잔여 자원 가중치
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Cost")
	float weightRemainingResource = 0.f;

	//예상 피격 피해 가중치
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Risk")
	float weightIncomingDanger = -0.5f;

	//점수 노이즈 폭
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Noise")
	float noiseTemperature = 0.0f;
};
