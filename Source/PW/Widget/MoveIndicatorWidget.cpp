// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/MoveIndicatorWidget.h"
#include "Components/TextBlock.h"

void UMoveIndicatorWidget::UpdateDistance(float Meters)
{
	if (DistanceText)
	{
		DistanceText->SetText(FText::FromString(FString::Printf(TEXT("%.1f m"), Meters)));
	}
}