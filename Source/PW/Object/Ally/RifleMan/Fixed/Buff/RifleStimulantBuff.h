//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "RifleStimulantBuff.generated.h"

//각성제 투여가 부여하는 버프, 1턴간 전투 능력을 전반적으로 끌어올린다
UCLASS()
class PW_API URifleStimulantBuff : public UBuffBase
{
	GENERATED_BODY()

public:
	URifleStimulantBuff();
};
