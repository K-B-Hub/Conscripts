//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/StoryRouteSelectWidget.h"
#include "Widget/StoryRouteButton.h"
#include "PlayerController/HubController.h"
#include "GameInstance/PWGameInstance.h"
#include "DataAsset/StoryRouteData.h"
#include "Run/PWSaveGame.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"

void UStoryRouteSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackButton) BackButton->OnClicked.AddDynamic(this, &UStoryRouteSelectWidget::HandleBackClicked);

	BuildRouteList();
}

void UStoryRouteSelectWidget::BuildRouteList()
{
	if (!RouteContainer || !routeButtonClass) return;

	const UWorld* world = GetWorld();
	const UPWGameInstance* gameInstance = world ? world->GetGameInstance<UPWGameInstance>() : nullptr;
	if (!gameInstance) return;

	const UPWSaveGame* save = gameInstance->GetSaveGame();

	//재진입 시 중복 생성 방지
	RouteContainer->ClearChildren();

	for (const UStoryRouteData* route : gameInstance->GetStoryRoutes())
	{
		if (!route) continue;

		UStoryRouteButton* button = CreateWidget<UStoryRouteButton>(this, routeButtonClass);
		if (!button) continue;

		button->InitRoute(route, save && save->IsStoryRouteCleared(route->routeId));
		button->OnClicked.BindUObject(this, &UStoryRouteSelectWidget::HandleRouteClicked);
		RouteContainer->AddChild(button);
	}
}

void UStoryRouteSelectWidget::HandleRouteClicked(FName routeId)
{
	if (AHubController* hub = GetOwningPlayer<AHubController>())
	{
		hub->ChooseStoryRoute(routeId);
	}
}

void UStoryRouteSelectWidget::HandleBackClicked()
{
	if (AHubController* hub = GetOwningPlayer<AHubController>())
	{
		hub->ShowModeSelect();
	}
}