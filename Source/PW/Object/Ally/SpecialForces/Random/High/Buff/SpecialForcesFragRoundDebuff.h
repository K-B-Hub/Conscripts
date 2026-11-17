//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "SpecialForcesFragRoundDebuff.generated.h"

//파쇄탄이 부여하는 디버프, 방어가 줄어든다
UCLASS()
class PW_API USpecialForcesFragRoundDebuff : public UBuffBase
{
	GENERATED_BODY()

public:
	USpecialForcesFragRoundDebuff();
};
