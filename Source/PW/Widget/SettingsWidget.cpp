//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SettingsWidget.h"
#include "PlayerController/HubController.h"
#include "Components/Button.h"

void USettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackButton) BackButton->OnClicked.AddDynamic(this, &USettingsWidget::HandleBackClicked);
}

void USettingsWidget::HandleBackClicked()
{
	if (AHubController* hub = GetOwningPlayer<AHubController>())
	{
		hub->ShowMainMenu();
	}
}