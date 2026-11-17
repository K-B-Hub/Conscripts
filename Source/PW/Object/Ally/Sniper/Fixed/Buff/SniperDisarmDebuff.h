//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "SniperDisarmDebuff.generated.h"

//무력화 사격이 부여하는 디버프, 명중이 줄어든다
UCLASS()
class PW_API USniperDisarmDebuff : public UBuffBase
{
	GENERATED_BODY()

public:
	USniperDisarmDebuff();
};
