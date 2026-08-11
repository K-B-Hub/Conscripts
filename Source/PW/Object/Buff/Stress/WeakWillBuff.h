//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "WeakWillBuff.generated.h"

//스트레스 부정 이벤트 '의지 박약', 이동력 감소
UCLASS()
class PW_API UWeakWillBuff : public UBuffBase
{
	GENERATED_BODY()

public:
	UWeakWillBuff();
};