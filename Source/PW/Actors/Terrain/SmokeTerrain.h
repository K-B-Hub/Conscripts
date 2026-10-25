//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Terrain/TerrainBase.h"
#include "SmokeTerrain.generated.h"

//연막탄이 생성하는 지형, 체류 중 회피가 올라가고 정해진 라운드 뒤 스스로 사라진다
//턴 훅이 없어 통과만 해도 손해가 없다
UCLASS()
class PW_API ASmokeTerrain : public ATerrainBase
{
	GENERATED_BODY()

public:
	ASmokeTerrain();
};