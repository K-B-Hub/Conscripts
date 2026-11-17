//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "SpecialForcesFlashBreachDebuff.generated.h"

//섬광 돌입이 부여하는 디버프, 명중과 회피가 줄어든다
UCLASS()
class PW_API USpecialForcesFlashBreachDebuff : public UBuffBase
{
	GENERATED_BODY()

public:
	USpecialForcesFlashBreachDebuff();
};
