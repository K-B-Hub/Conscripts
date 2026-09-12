//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/ModeSelectWidget.h"
#include "PlayerController/HubController.h"
#include "GameInstance/PWGameInstance.h"
#include "Run/PWSaveGame.h"
#include "Components/Button.h"

void UModeSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StoryButton)     StoryButton->OnClicked.AddDynamic(this, &UModeSelectWidget::HandleStoryClicked);
	if (RoguelikeButton) RoguelikeButton->OnClicked.AddDynamic(this, &UModeSelectWidget::HandleRoguelikeClicked);
	if (NightmareButton) NightmareButton->OnClicked.AddDynamic(this, &UModeSelectWidget::HandleNightmareClicked);
	if (BackButton)      BackButton->OnClicked.AddDynamic(this, &UModeSelectWidget::HandleBackClicked);

	ApplyUnlockState();
}

void UModeSelectWidget::ApplyUnlockState()
{
	const UWorld* world = GetWorld();
	const UPWGameInstance* gameInstance = world ? world->GetGameInstance<UPWGameInstance>() : nullptr;
	const UPWSaveGame* save = gameInstance ? gameInstance->GetSaveGame() : nullptr;
	if (!save) return;

	//스토리는 항상 개방이므로 나머지 둘만 판정
	if (RoguelikeButton) RoguelikeButton->SetIsEnabled(save->IsModeUnlocked(EGameDifficulty::Roguelike));
	if (NightmareButton) NightmareButton->SetIsEnabled(save->IsModeUnlocked(EGameDifficulty::Nightmare));
}

void UModeSelectWidget::HandleStoryClicked()
{
	if (AHubController* hub = GetOwningPlayer<AHubController>())
	{
		hub->ChooseMode(EGameDifficulty::Stage);
	}
}

void UModeSelectWidget::HandleRoguelikeClicked()
{
	if (AHubController* hub = GetOwningPlayer<AHubController>())
	{
		hub->ChooseMode(EGameDifficulty::Roguelike);
	}
}

void UModeSelectWidget::HandleNightmareClicked()
{
	if (AHubController* hub = GetOwningPlayer<AHubController>())
	{
		hub->ChooseMode(EGameDifficulty::Nightmare);
	}
}

void UModeSelectWidget::HandleBackClicked()
{
	if (AHubController* hub = GetOwningPlayer<AHubController>())
	{
		hub->ShowMainMenu();
	}
}