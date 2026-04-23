// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/MoveWidget.h"
#include "Components/Button.h"
#include "PlayerController/BattleController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UMoveWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (MoveButton)
	{
		MoveButton->OnClicked.AddDynamic(this, &UMoveWidget::OnMoveButtonClicked);
	}
	SetIsFocusable(false);
}

void UMoveWidget::OnMoveButtonClicked()
{
	if (ABattleController* BC = Cast<ABattleController>(GetOwningPlayer()))
	{
		BC->ToggleMoveMode();
	}
	// 클릭 후 포커스를 게임 뷰포트로 반환 — 버튼에 포커스가 남아 스페이스바 오입력 방지
	UWidgetBlueprintLibrary::SetFocusToGameViewport();
}

void UMoveWidget::RefreshMoveButton(float currentMovingPoint)
{
	if (MoveButton)
	{
		MoveButton->SetIsEnabled(currentMovingPoint > 0.f);
	}
}