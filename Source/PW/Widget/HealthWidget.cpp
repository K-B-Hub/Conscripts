// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/HealthWidget.h"
#include "Components/ProgressBar.h"

void UHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(false);
}

void UHealthWidget::InitHealth(int32 InMaxHp, int32 InCurrentHp)
{
	maxHp = FMath::Max(1, InMaxHp);
	currentHp = FMath::Clamp(InCurrentHp, 0, maxHp);
	UpdateHealthBar();
}

void UHealthWidget::ApplyDamage(int32 Amount)
{
	currentHp = FMath::Clamp(currentHp - Amount, 0, maxHp);
	UpdateHealthBar();
}

void UHealthWidget::UpdateHealthBar()
{
	if (HealthBar)
	{
		HealthBar->SetPercent(static_cast<float>(currentHp) / static_cast<float>(maxHp));
	}
}