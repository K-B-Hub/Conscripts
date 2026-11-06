// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/TurnEndWidget.h"

#include "Components/Button.h"
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
		//행동 중이면 결과 확정까지 래치로 보류된다
		cachedController->RequestEndTurn();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TurnEnd Clicked"));
	}
}