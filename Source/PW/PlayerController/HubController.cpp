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

void AHubController::ShowFormation()
{
	SwapScreen(formationWidgetClass);
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

	gameInstance->StartRun(roster, pendingStoryRouteId, gameInstance->GetDefaultStageSequence());

	const URunProgress* runProgress = gameInstance->GetRunProgress();
	const FStageEntry* stage = runProgress ? runProgress->GetCurrentStage() : nullptr;
	if (!stage || stage->Map.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Hub] 스테이지 시퀀스가 비어 있어 이동할 수 없습니다"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[Hub] 편성 확정 %d명 — 스테이지 1로 이동"), roster.Num());
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, stage->Map);
}

void AHubController::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}