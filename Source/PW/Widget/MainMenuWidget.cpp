//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/MainMenuWidget.h"
#include "PlayerController/HubController.h"
#include "GameInstance/PWGameInstance.h"
#include "Components/Button.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton)    StartButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleStartClicked);
	if (ContinueButton) ContinueButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleContinueClicked);
	if (SettingsButton) SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleSettingsClicked);
	if (QuitButton)     QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleQuitClicked);

	//이어할 런이 없으면 누를 것이 없다
	if (ContinueButton)
	{
		const UWorld* world = GetWorld();
		const UPWGameInstance* gameInstance = world ? world->GetGameInstance<UPWGameInstance>() : nullptr;
		ContinueButton->SetIsEnabled(gameInstance && gameInstance->HasSavedRun());
	}
}

void UMainMenuWidget::HandleStartClicked()
{
	if (AHubController* hub = GetOwningPlayer<AHubController>())
	{
		hub->ShowModeSelect();
	}
}

void UMainMenuWidget::HandleContinueClicked()
{
	if (AHubController* hub = GetOwningPlayer<AHubController>())
	{
		hub->ContinueRun();
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