// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/TurnOrderWidget.h"

#include "Components/ScrollBox.h"
#include "GameMode/BattleGameMode.h"
#include "Widget/TurnOrderEntryWidget.h"

void UTurnOrderWidget::NativeConstruct()
{
	Super::NativeConstruct();

	cachedGameMode = Cast<ABattleGameMode>(GetWorld()->GetAuthGameMode());
	if (cachedGameMode)
	{
		cachedGameMode->OnTurnOrderRebuilt.AddUObject(this, &UTurnOrderWidget::RebuildEntries);
		cachedGameMode->OnTurnChanged.AddUObject(this, &UTurnOrderWidget::SetCurrentTurn);

		//위젯 생성 시점에 이미 진행 중인 전투 상태 반영
		RebuildEntries(cachedGameMode->GetTurnOrder(), cachedGameMode->GetCurrentTurnIndex());
	}
}

void UTurnOrderWidget::NativeDestruct()
{
	if (cachedGameMode)
	{
		cachedGameMode->OnTurnOrderRebuilt.RemoveAll(this);
		cachedGameMode->OnTurnChanged.RemoveAll(this);
		cachedGameMode = nullptr;
	}
	Super::NativeDestruct();
}

void UTurnOrderWidget::RebuildEntries(const TArray<ACharacterBase*>& InTurnOrder, int32 InCurrentIndex)
{
	if (!TurnScrollBox || !EntryClass) return;

	TurnScrollBox->ClearChildren();
	entries.Empty();

	for (ACharacterBase* Character : InTurnOrder)
	{
		UTurnOrderEntryWidget* entry = CreateWidget<UTurnOrderEntryWidget>(this, EntryClass);
		entry->InitEntry(Character);
		TurnScrollBox->AddChild(entry);
		entries.Add(entry);
	}

	SetCurrentTurn(InCurrentIndex);
}

void UTurnOrderWidget::SetCurrentTurn(int32 InCurrentIndex)
{
	for (int32 i = 0; i < entries.Num(); ++i)
	{
		if (i < InCurrentIndex)
		{
			entries[i]->SetState(ETurnEntryState::Past);
		}
		else if (i == InCurrentIndex)
		{
			entries[i]->SetState(ETurnEntryState::Current);
		}
		else
		{
			entries[i]->SetState(ETurnEntryState::Upcoming);
		}
	}

	//현재 턴 칸이 항상 보이도록 스크롤
	if (entries.IsValidIndex(InCurrentIndex))
	{
		TurnScrollBox->ScrollWidgetIntoView(entries[InCurrentIndex]);
	}
}
