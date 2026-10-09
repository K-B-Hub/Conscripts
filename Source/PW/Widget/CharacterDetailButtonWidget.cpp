//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/CharacterDetailButtonWidget.h"
#include "Components/Button.h"

void UCharacterDetailButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (DetailButton) DetailButton->OnClicked.AddUniqueDynamic(this, &UCharacterDetailButtonWidget::HandleClicked);
}

void UCharacterDetailButtonWidget::HandleClicked()
{
	OnDetailClicked.ExecuteIfBound();
}