//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "SniperIncapacitateDebuff.generated.h"

//무력화가 부여하는 디버프, 행동력이 줄어든다
UCLASS()
class PW_API USniperIncapacitateDebuff : public UBuffBase
{
	GENERATED_BODY()

public:
	USniperIncapacitateDebuff();
};
