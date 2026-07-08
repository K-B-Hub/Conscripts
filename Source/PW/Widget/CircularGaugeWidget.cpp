// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/CircularGaugeWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"

void UCircularGaugeWidget::InitGauge(int32 InMax, int32 InCurrent)
{
	maxValue = FMath::Max(InMax, 1);
	currentValue = InCurrent;
	UpdateGauge();
}

void UCircularGaugeWidget::SetValue(int32 InCurrent)
{
	currentValue = InCurrent;
	UpdateGauge();
}

void UCircularGaugeWidget::UpdateGauge()
{
	//GetDynamicMaterial은 첫 호출 시 MID를 생성·캐시하므로 매 갱신 호출 안전
	if (GaugeImage)
	{
		if (UMaterialInstanceDynamic* mid = GaugeImage->GetDynamicMaterial())
		{
			mid->SetScalarParameterValue(TEXT("Percent"), (float)currentValue / maxValue);
		}
	}

	if (ValueText)
	{
		ValueText->SetText(FText::AsNumber(currentValue));
	}
	if (MaxValueText)
	{
		MaxValueText->SetText(FText::FromString(FString::Printf(TEXT("/ %d"), maxValue)));
	}
}
