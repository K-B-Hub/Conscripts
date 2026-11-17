//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "SniperShoulderShotDebuff.generated.h"

//어깨 사격이 부여하는 디버프, 명중과 치명이 줄어든다
UCLASS()
class PW_API USniperShoulderShotDebuff : public UBuffBase
{
	GENERATED_BODY()

public:
	USniperShoulderShotDebuff();
};
