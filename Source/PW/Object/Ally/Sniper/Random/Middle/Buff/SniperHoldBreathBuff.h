//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "SniperHoldBreathBuff.generated.h"

//숨 참기가 부여하는 자기 버프
UCLASS()
class PW_API USniperHoldBreathBuff : public UBuffBase
{
	GENERATED_BODY()

public:
	USniperHoldBreathBuff();
};
