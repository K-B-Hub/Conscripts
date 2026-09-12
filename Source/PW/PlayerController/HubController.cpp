//Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerController/HubController.h"
#include "Widget/MainMenuWidget.h"
#include "Widget/ModeSelectWidget.h"
#include "Widget/StoryRouteSelectWidget.h"
#include "Widget/SettingsWidget.h"
#include "GameInstance/PWGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetSystemLibrary.h"

void AHubController::BeginPlay()
{
	Super::BeginPlay();

	//메뉴는 마우스 전용, 게임 입력을 받지 않음
	SetShowMouseCursor(true);
	SetInputMode(FInputModeUIOnly());

	ShowMainMenu();
}

void AHubController::SwapScreen(TSubclassOf<UUserWidget> widgetClass)
{
	if (currentWidget)
	{
		currentWidget->RemoveFromParent();
		currentWidget = nullptr;
	}

	if (!widgetClass) return;

	UUserWidget* widget = CreateWidget<UUserWidget>(this, widgetClass);
	if (!widget) return;

	widget->AddToViewport();
	currentWidget = widget;
}

void AHubController::ShowMainMenu()
{
	SwapScreen(mainMenuWidgetClass);
}

void AHubController::ShowModeSelect()
{
	SwapScreen(modeSelectWidgetClass);
}

void AHubController::ShowStoryRouteSelect()
{
	SwapScreen(storyRouteSelectWidgetClass);
}

void AHubController::ShowSettings()
{
	SwapScreen(settingsWidgetClass);
}

void AHubController::ChooseMode(EGameDifficulty mode)
{
	UPWGameInstance* gameInstance = GetGameInstance<UPWGameInstance>();
	if (!gameInstance) return;

	gameInstance->SetDifficulty(mode);

	//스토리는 줄기를 먼저 고르고, 나머지는 바로 편성으로
	if (mode == EGameDifficulty::Stage)
	{
		ShowStoryRouteSelect();
		return;
	}

	//모드를 되돌려 고를 수 있으므로 이전 선택이 남지 않게 비운다
	pendingStoryRouteId = NAME_None;

	//TODO: 편성 화면 구현 후 ShowFormation()으로 교체
	UE_LOG(LogTemp, Log, TEXT("[Hub] 모드 선택: %d — 편성 화면 미구현"), static_cast<int32>(mode));
}

void AHubController::ChooseStoryRoute(FName routeId)
{
	pendingStoryRouteId = routeId;

	//TODO: 편성 화면 구현 후 ShowFormation()으로 교체
	UE_LOG(LogTemp, Log, TEXT("[Hub] 스토리 줄기 선택: %s — 편성 화면 미구현"), *routeId.ToString());
}

void AHubController::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}