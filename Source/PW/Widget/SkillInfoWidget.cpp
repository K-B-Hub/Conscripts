// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SkillInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"

void USkillInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(false);
}

void USkillInfoWidget::UpdateInfo(float Damage, float Accuracy, float Critical, ESkillType SkillType)
{
	if (SkillType == ESkillType::Buff)
	{
		if (DamageText)
		{
			DamageText->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (AccuracyText)
		{
			AccuracyText->SetVisibility(ESlateVisibility::Visible);
			AccuracyText->SetText(FText::FromString(FString::Printf(TEXT("Accuracy: %d%%"), FMath::RoundToInt(Accuracy))));
		}
		if (CriticalText)
		{
			CriticalText->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (BuffText)
		{
			BuffText->SetVisibility(ESlateVisibility::Visible);
		}
		if (AilmentText)
		{
			AilmentText->SetVisibility(ESlateVisibility::Collapsed);
		}
		return;
	}

	if (SkillType == ESkillType::Ailment)
	{
		if (DamageText)
		{
			DamageText->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (AccuracyText)
		{
			AccuracyText->SetVisibility(ESlateVisibility::Visible);
			AccuracyText->SetText(FText::FromString(FString::Printf(TEXT("Accuracy: %d%%"), FMath::RoundToInt(Accuracy))));
		}
		if (CriticalText)
		{
			CriticalText->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (BuffText)
		{
			BuffText->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (AilmentText)
		{
			AilmentText->SetVisibility(ESlateVisibility::Visible);
		}
		return;
	}

	if (DamageText)
	{
		DamageText->SetVisibility(ESlateVisibility::Visible);
		if (Damage < 0.f)
		{
			DamageText->SetText(FText::FromString(FString::Printf(TEXT("Heal: %d"), FMath::RoundToInt(Damage * -1))));
		}
		else
		{
			DamageText->SetText(FText::FromString(FString::Printf(TEXT("Damage: %d"), FMath::RoundToInt(Damage))));
		}
	}
	if (AccuracyText)
	{
		AccuracyText->SetVisibility(ESlateVisibility::Visible);
		AccuracyText->SetText(FText::FromString(FString::Printf(TEXT("Accuracy: %d%%"), FMath::RoundToInt(Accuracy))));
	}
	if (CriticalText)
	{
		if (Damage < 0.f)
		{
			CriticalText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			CriticalText->SetVisibility(ESlateVisibility::Visible);
			CriticalText->SetText(FText::FromString(FString::Printf(TEXT("Critical: %d%%"), FMath::RoundToInt(Critical))));
		}
	}
	if (BuffText)
	{
		BuffText->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (AilmentText)
	{
		AilmentText->SetVisibility(ESlateVisibility::Collapsed);
	}
}
