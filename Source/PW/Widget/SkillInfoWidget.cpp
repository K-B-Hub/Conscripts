// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SkillInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"

void USkillInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(false);
}

void USkillInfoWidget::UpdateInfo(float Damage, float DamageMax, float Accuracy, float Critical, ESkillType SkillType)
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

		//반올림 후 비교해야 표시상 같은 값이 "5~5"로 나오지 않는다
		const int32 minValue = FMath::RoundToInt(Damage);
		const int32 maxValue = FMath::RoundToInt(DamageMax);
		const bool bRanged = maxValue > minValue;

		if (Damage < 0.f)
		{
			//회복은 부호를 뒤집어 표기하므로 구간의 앞뒤도 뒤바뀐다
			DamageText->SetText(bRanged
				? FText::FromString(FString::Printf(TEXT("Heal: %d~%d"), -maxValue, -minValue))
				: FText::FromString(FString::Printf(TEXT("Heal: %d"), -minValue)));
		}
		else
		{
			DamageText->SetText(bRanged
				? FText::FromString(FString::Printf(TEXT("Damage: %d~%d"), minValue, maxValue))
				: FText::FromString(FString::Printf(TEXT("Damage: %d"), minValue)));
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
