//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Ally/RifleMan/Basic/RifleReload.h"
#include "SniperReload.generated.h"

//저격수 기본 스킬, 탄환을 보충한다
//동작은 소총수 재장전을 그대로 쓰고 이름과 보충량만 바꾼다
UCLASS()
class PW_API USniperReload : public URifleReload
{
	GENERATED_BODY()

public:
	USniperReload();
};
