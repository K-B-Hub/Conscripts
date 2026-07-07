//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "TestAtkBuff.generated.h"

//테스트용 공격력 버프
UCLASS()
class PW_API UTestAtkBuff : public UBuffBase
{
	GENERATED_BODY()

public:
	UTestAtkBuff();
};
