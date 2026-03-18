// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/TurnEndWidget.h"

#include "../../../../../UE_5.6/Engine/Source/Runtime/Engine/Classes/AI/NavigationSystemBase.h"
#include "Components/Button.h"
#include "GameMode/BattleGameMode.h"
#include "PlayerController/BattleController.h"

void UTurnEndWidget::NativeConstruct()
{
	Super::NativeConstruct();

	cachedGameMode = Cast<ABattleGameMode>(GetWorld()->GetAuthGameMode());
	cachedController = Cast<ABattleController>(GetWorld()->GetFirstPlayerController());
	
	if (turnEndButton)
	{
		turnEndButton->OnClicked.AddDynamic(this, &UTurnEndWidget::OnTurnEndClicked);
	}
}

void UTurnEndWidget::OnTurnEndClicked()
{
	if (cachedGameMode && cachedController)
	{
		cachedController->EndTurn();
		cachedGameMode->OnTurnEnd();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TurnEnd Clicked"));
	}
}