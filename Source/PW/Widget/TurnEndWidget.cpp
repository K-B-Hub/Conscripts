// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/TurnEndWidget.h"

#include "Widget/CircularButtonWidget.h"
#include "GameMode/BattleGameMode.h"
#include "PlayerController/BattleController.h"

void UTurnEndWidget::NativeConstruct()
{
	Super::NativeConstruct();

	cachedGameMode = Cast<ABattleGameMode>(GetWorld()->GetAuthGameMode());
	cachedController = Cast<ABattleController>(GetWorld()->GetFirstPlayerController());
	SetIsFocusable(false);
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