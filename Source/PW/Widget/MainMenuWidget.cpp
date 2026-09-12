//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/MainMenuWidget.h"
#include "PlayerController/HubController.h"
#include "Components/Button.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton)    StartButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleStartClicked);
	if (SettingsButton) SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleSettingsClicked);
	if (QuitButton)     QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleQuitClicked);
}

void UMainMenuWidget::HandleStartClicked()
{
	if (AHubController* hub = GetOwningPlayer<AHubController>())
	{
		hub->ShowModeSelect();
	}
}

void UMainMenuWidget::HandleSettingsClicked()
{
	if (AHubController* hub = GetOwningPlayer<AHubController>())
	{
		hub->ShowSettings();
	}
}

void UMainMenuWidget::HandleQuitClicked()
{
	if (AHubController* hub = GetOwningPlayer<AHubController>())
	{
		hub->QuitGame();
	}
}