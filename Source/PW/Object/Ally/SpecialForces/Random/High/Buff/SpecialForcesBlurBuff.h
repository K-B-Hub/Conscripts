//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "SpecialForcesBlurBuff.generated.h"

//잔상이 부여하는 자기 버프, 최대 3중첩
UCLASS()
class PW_API USpecialForcesBlurBuff : public UBuffBase
{
	GENERATED_BODY()

public:
	USpecialForcesBlurBuff();
};
