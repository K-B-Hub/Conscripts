//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "SpecialForcesCounterStanceBuff.generated.h"

//역습 태세가 부여하는 자기 버프
UCLASS()
class PW_API USpecialForcesCounterStanceBuff : public UBuffBase
{
	GENERATED_BODY()

public:
	USpecialForcesCounterStanceBuff();
};
