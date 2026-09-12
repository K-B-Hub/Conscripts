//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/StoryRouteButton.h"
#include "DataAsset/StoryRouteData.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UStoryRouteButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (RouteButton) RouteButton->OnClicked.AddDynamic(this, &UStoryRouteButton::HandleClicked);
}

void UStoryRouteButton::InitRoute(const UStoryRouteData* route, bool bCleared)
{
	if (!route) return;

	routeId = route->routeId;

	if (NameText) NameText->SetText(route->displayName);

	if (ClearedMark)
	{
		ClearedMark->SetVisibility(bCleared ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UStoryRouteButton::HandleClicked()
{
	OnClicked.ExecuteIfBound(routeId);
}