//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Ally/RifleMan/Basic/RifleReload.h"
#include "SpecialForcesResupply.generated.h"

//특수부대원 기본 스킬, 장비를 보충한다
//동작은 소총수 재장전을 그대로 쓰고 이름과 보충량만 바꾼다
UCLASS()
class PW_API USpecialForcesResupply : public URifleReload
{
	GENERATED_BODY()

public:
	USpecialForcesResupply();
};
