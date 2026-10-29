//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/StoryRouteButton.h"
#include "DataAsset/StoryRouteData.h"
#include "Widget/ImageButton.h"

void UStoryRouteButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (RouteButton) RouteButton->OnClicked.AddDynamic(this, &UStoryRouteButton::HandleClicked);
}

void UStoryRouteButton::InitRoute(const UStoryRouteData* route)
{
	if (!route) return;

	routeId = route->routeId;

	//줄기별 아트는 DataAsset이 들고 있다
	if (RouteButton) RouteButton->SetStateTextures(route->buttonArt);

	//클리어 표식은 띄우지 않는다
	if (ClearedMark) ClearedMark->SetVisibility(ESlateVisibility::Collapsed);
}

void UStoryRouteButton::HandleClicked()
{
	OnClicked.ExecuteIfBound(routeId);
}