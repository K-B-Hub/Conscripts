//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "SpecialForcesCounterChanceBuff.generated.h"

//역공의 기회가 부여하는 자기 버프
UCLASS()
class PW_API USpecialForcesCounterChanceBuff : public UBuffBase
{
	GENERATED_BODY()

public:
	USpecialForcesCounterChanceBuff();
};
