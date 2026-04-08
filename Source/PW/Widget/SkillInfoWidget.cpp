// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SkillInfoWidget.h"
#include "Components/TextBlock.h"

void USkillInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(false);
}

void USkillInfoWidget::UpdateInfo(float Damage, float Accuracy, float Critical)
{
	if (DamageText)
	{
		DamageText->SetText(FText::FromString(FString::Printf(TEXT("DMG: %d"), FMath::RoundToInt(Damage))));
	}
	if (AccuracyText)
	{
		AccuracyText->SetText(FText::FromString(FString::Printf(TEXT("HIT: %d%%"), FMath::RoundToInt(Accuracy))));
	}
	if (CriticalText)
	{
		CriticalText->SetText(FText::FromString(FString::Printf(TEXT("CRIT: %d%%"), FMath::RoundToInt(Critical))));
	}
}
