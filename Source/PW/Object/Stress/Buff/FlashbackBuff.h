//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "FlashbackBuff.generated.h"

//스트레스 부정 이벤트 '플래시백', 이동력과 명중·회피 감소
UCLASS()
class PW_API UFlashbackBuff : public UBuffBase
{
	GENERATED_BODY()

public:
	UFlashbackBuff();
};