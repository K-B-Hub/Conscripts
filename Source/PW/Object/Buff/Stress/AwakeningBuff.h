//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "AwakeningBuff.generated.h"

//스트레스 긍정 이벤트 '각성', 전투 파생 스탯 일괄 상승
UCLASS()
class PW_API UAwakeningBuff : public UBuffBase
{
	GENERATED_BODY()

public:
	UAwakeningBuff();
};