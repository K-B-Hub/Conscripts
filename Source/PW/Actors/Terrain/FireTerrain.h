//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Terrain/LavaTerrain.h"
#include "FireTerrain.generated.h"

//화염병이 생성하는 지형, 체류 중 턴마다 피해를 주고 정해진 라운드 뒤 스스로 사라진다
//피해 훅은 용암과 동일하므로 수치와 수명만 바꿔 쓴다
UCLASS()
class PW_API AFireTerrain : public ALavaTerrain
{
	GENERATED_BODY()

public:
	AFireTerrain();
};
