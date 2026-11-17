//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "SniperExposeWeaknessDebuff.generated.h"

//약점 포착이 부여하는 디버프, 받는 피해가 늘어난다
UCLASS()
class PW_API USniperExposeWeaknessDebuff : public UBuffBase
{
	GENERATED_BODY()

public:
	USniperExposeWeaknessDebuff();
};
