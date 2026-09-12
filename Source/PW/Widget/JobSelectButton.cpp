//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/JobSelectButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UJobSelectButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (JobButton) JobButton->OnClicked.AddDynamic(this, &UJobSelectButton::HandleClicked);
}

void UJobSelectButton::InitEntry(const FText& label, int32 index)
{
	entryIndex = index;

	if (LabelText) LabelText->SetText(label);
}

void UJobSelectButton::HandleClicked()
{
	OnClicked.ExecuteIfBound(entryIndex);
}