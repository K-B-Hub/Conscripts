//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "DepressionBuff.generated.h"

//스트레스 부정 이벤트 '우울증', 이동력과 행동력 감소
UCLASS()
class PW_API UDepressionBuff : public UBuffBase
{
	GENERATED_BODY()

public:
	UDepressionBuff();
};