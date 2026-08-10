//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "HeroicSpiritBuff.generated.h"

//스트레스 긍정 이벤트 '영웅의 기개', 주요 전투 스탯 일괄 상승
UCLASS()
class PW_API UHeroicSpiritBuff : public UBuffBase
{
	GENERATED_BODY()

public:
	UHeroicSpiritBuff();
};