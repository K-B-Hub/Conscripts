//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Terrain/TerrainBase.h"
#include "LavaTerrain.generated.h"

//체류 중 턴을 시작하거나 끝낼 때마다 피해를 주는 지형
UCLASS()
class PW_API ALavaTerrain : public ATerrainBase
{
	GENERATED_BODY()

public:
	//턴 시작·종료 시 각각 입는 피해량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Lava")
	int32 damagePerTurn = 2;

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnCharacterTurnStart(ACharacterBase* character) override;
	virtual void OnCharacterTurnEnd(ACharacterBase* character) override;
};