//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "SpecialForcesDistractionDebuff.generated.h"

//교란이 부여하는 디버프, 명중이 줄어든다
UCLASS()
class PW_API USpecialForcesDistractionDebuff : public UBuffBase
{
	GENERATED_BODY()

public:
	USpecialForcesDistractionDebuff();
};
