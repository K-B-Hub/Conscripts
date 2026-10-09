//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/PauseMenuWidget.h"
#include "PlayerController/BattleController.h"
#include "Components/Button.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResumeButton)         ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::HandleResumeClicked);
	if (SquadInfoButton)      SquadInfoButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::HandleSquadInfoClicked);
	if (SettingsButton)       SettingsButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::HandleSettingsClicked);
	if (MainMenuButton)        MainMenuButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::HandleMainMenuClicked);
	if (MainMenuConfirmButton) MainMenuConfirmButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::HandleMainMenuConfirmClicked);
	if (MainMenuCancelButton)  MainMenuCancelButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::HandleMainMenuCancelClicked);
	if (AbandonButton)         AbandonButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::HandleAbandonClicked);
	if (AbandonConfirmButton)  AbandonConfirmButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::HandleAbandonConfirmClicked);
	if (AbandonCancelButton)   AbandonCancelButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::HandleAbandonCancelClicked);

	CollapseConfirmPanels();
}

void UPauseMenuWidget::CollapseConfirmPanels()
{
	if (MainMenuConfirmPanel) MainMenuConfirmPanel->SetVisibility(ESlateVisibility::Collapsed);
	if (AbandonConfirmPanel)  AbandonConfirmPanel->SetVisibility(ESlateVisibility::Collapsed);
}

void UPauseMenuWidget::HandleResumeClicked()
{
	if (ABattleController* battle = GetOwningPlayer<ABattleController>())
	{
		battle->ResumeFromPause();
	}
}

void UPauseMenuWidget::HandleSquadInfoClicked()
{
	if (ABattleController* battle = GetOwningPlayer<ABattleController>())
	{
		battle->ShowSquadInfoFromPause();
	}
}

void UPauseMenuWidget::HandleSettingsClicked()
{
	if (ABattleController* battle = GetOwningPlayer<ABattleController>())
	{
		battle->ShowSettingsFromPause();
	}
}

void UPauseMenuWidget::HandleMainMenuClicked()
{
	CollapseConfirmPanels();
	if (MainMenuConfirmPanel) MainMenuConfirmPanel->SetVisibility(ESlateVisibility::Visible);
}

void UPauseMenuWidget::HandleMainMenuCancelClicked()
{
	CollapseConfirmPanels();
}

void UPauseMenuWidget::HandleMainMenuConfirmClicked()
{
	if (ABattleController* battle = GetOwningPlayer<ABattleController>())
	{
		battle->ReturnToMainMenu();
	}
}

void UPauseMenuWidget::HandleAbandonClicked()
{
	CollapseConfirmPanels();
	if (AbandonConfirmPanel) AbandonConfirmPanel->SetVisibility(ESlateVisibility::Visible);
}

void UPauseMenuWidget::HandleAbandonCancelClicked()
{
	CollapseConfirmPanels();
}

void UPauseMenuWidget::HandleAbandonConfirmClicked()
{
	if (ABattleController* battle = GetOwningPlayer<ABattleController>())
	{
		battle->AbandonRun();
	}
}