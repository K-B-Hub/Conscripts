//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "SniperLegShotDebuff.generated.h"

//다리 사격이 부여하는 디버프, 이동력이 줄어든다
UCLASS()
class PW_API USniperLegShotDebuff : public UBuffBase
{
	GENERATED_BODY()

public:
	USniperLegShotDebuff();
};
