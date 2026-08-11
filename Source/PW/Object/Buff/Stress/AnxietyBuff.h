//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "AnxietyBuff.generated.h"

//스트레스 부정 이벤트 '불안장애', 명중·회피 대폭 감소
UCLASS()
class PW_API UAnxietyBuff : public UBuffBase
{
	GENERATED_BODY()

public:
	UAnxietyBuff();
};