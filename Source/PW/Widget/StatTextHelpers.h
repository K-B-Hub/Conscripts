//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"

//스탯 칸 하나를 채우는 헬퍼, 널 검사를 호출부마다 쓰지 않으려고 모았다
//상세 정보 창·부대 정보표·호버 미리보기가 같은 포맷을 쓴다
namespace PWStatText
{
	inline void SetInt(UTextBlock* text, int32 value)
	{
		if (text) text->SetText(FText::AsNumber(value));
	}

	inline void SetFloat(UTextBlock* text, float value)
	{
		if (text) text->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), value)));
	}

	inline void SetPair(UTextBlock* text, int32 current, int32 max)
	{
		if (text) text->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), current, max)));
	}
}