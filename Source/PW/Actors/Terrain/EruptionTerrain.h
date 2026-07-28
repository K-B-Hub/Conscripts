//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Terrain/TerrainBase.h"
#include "EruptionTerrain.generated.h"

//체류 중 턴 시작 시 일정 확률로 분출해 주변에 피해를 주는 지형
UCLASS()
class PW_API AEruptionTerrain : public ATerrainBase
{
	GENERATED_BODY()

public:
	//분출 확률(%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Eruption")
	int32 eruptionChance = 30;

	//분출 시 피해량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Eruption")
	int32 eruptionDamage = 5;

	//분출 반경(cm), 지형 밖도 휩쓸림
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Eruption")
	float eruptionRadius = 400.f;

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnCharacterTurnStart(ACharacterBase* character) override;
};