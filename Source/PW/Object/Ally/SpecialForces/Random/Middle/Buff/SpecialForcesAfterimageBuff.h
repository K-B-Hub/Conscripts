//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "SpecialForcesAfterimageBuff.generated.h"

//잔영이 부여하는 자기 버프, 최대 3중첩
UCLASS()
class PW_API USpecialForcesAfterimageBuff : public UBuffBase
{
	GENERATED_BODY()

public:
	USpecialForcesAfterimageBuff();
};
