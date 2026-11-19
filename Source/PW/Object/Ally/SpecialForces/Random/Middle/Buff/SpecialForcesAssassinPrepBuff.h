//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "SpecialForcesAssassinPrepBuff.generated.h"

//암살 준비가 부여하는 자기 버프
UCLASS()
class PW_API USpecialForcesAssassinPrepBuff : public UBuffBase
{
	GENERATED_BODY()

public:
	USpecialForcesAssassinPrepBuff();
};
