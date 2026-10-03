//Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerController/HubController.h"
#include "Widget/MainMenuWidget.h"
#include "Widget/ModeSelectWidget.h"
#include "Widget/StoryRouteSelectWidget.h"
#include "Widget/SettingsWidget.h"
#include "Widget/FormationWidget.h"
#include "GameInstance/PWGameInstance.h"
#include "Run/RunProgress.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

void AHubController::BeginPlay()
{
	Super::BeginPlay();

	//메뉴는 마우스 전용, 게임 입력을 받지 않음
	SetShowMouseCursor(true);
	SetInputMode(FInputModeUIOnly());

	ShowMainMenu();
}

UUserWidget* AHubController::SwapScreen(TSubclassOf<UUserWidget> widgetClass)
{
	if (currentWidget)
	{
		currentWidget->RemoveFromParent();
		currentWidget = nullptr;
	}

	if (!widgetClass) return nullptr;

	UUserWidget* widget = CreateWidget<UUserWidget>(this, widgetClass);
	if (!widget) return nullptr;

	widget->AddToViewport();
	currentWidget = widget;
	return widget;
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
	//허브에서 열면 뒤로가기가 메인메뉴로 간다, 전투 일시정지에서 열면 그쪽이 다르게 바인딩한다
	if (USettingsWidget* settings = Cast<USettingsWidget>(SwapScreen(settingsWidgetClass)))
	{
		settings->onClosed.BindUObject(this, &AHubController::ShowMainMenu);
	}
}

void AHubController::ShowFormation()
{
	SwapScreen(formationWidgetClass);
}

void AHubController::ContinueRun()
{
	UPWGameInstance* gameInstance = GetGameInstance<UPWGameInstance>();
	if (!gameInstance) return;

	//실패하면 레벨 전환이 일어나지 않아 메뉴에 그대로 머문다
	if (!gameInstance->ContinueSavedRun(this))
	{
		UE_LOG(LogTemp, Warning, TEXT("[HubController] 런 이어하기 실패"));
	}
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

	ShowFormation();
}

void AHubController::ChooseStoryRoute(FName routeId)
{
	pendingStoryRouteId = routeId;

	ShowFormation();
}

void AHubController::ConfirmFormation(const TArray<FAllyRunState>& roster)
{
	UPWGameInstance* gameInstance = GetGameInstance<UPWGameInstance>();
	if (!gameInstance) return;

	gameInstance->StartRun(roster, pendingStoryRouteId);

	UE_LOG(LogTemp, Log, TEXT("[Hub] 편성 확정 %d명"), roster.Num());
	gameInstance->TravelToCurrentStage(this);
}

void AHubController::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}