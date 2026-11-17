//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "SniperTargetMarkDebuff.generated.h"

//조준 표식이 부여하는 디버프, 회피가 줄어든다
UCLASS()
class PW_API USniperTargetMarkDebuff : public UBuffBase
{
	GENERATED_BODY()

public:
	USniperTargetMarkDebuff();
};
