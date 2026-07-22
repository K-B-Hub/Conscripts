// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/DebugWidget.h"
#include "Components/Button.h"
#include "PlayerController/BattleController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UDebugWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetIsFocusable(false);

	if (AddStressButton)
	{
		AddStressButton->OnClicked.AddDynamic(this, &UDebugWidget::OnAddStressClicked);
	}
	if (LevelUpButton)
	{
		LevelUpButton->OnClicked.AddDynamic(this, &UDebugWidget::OnLevelUpClicked);
	}
}

void UDebugWidget::OnAddStressClicked()
{
	if (ABattleController* BC = Cast<ABattleController>(GetOwningPlayer()))
	{
		BC->DebugAddStress();
	}
	UWidgetBlueprintLibrary::SetFocusToGameViewport();
}

void UDebugWidget::OnLevelUpClicked()
{
	if (ABattleController* BC = Cast<ABattleController>(GetOwningPlayer()))
	{
		BC->DebugLevelUp();
	}
	UWidgetBlueprintLibrary::SetFocusToGameViewport();
}
