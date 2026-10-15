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

	//칸만 만들어 두고 스크롤박스 배치는 SetCurrentTurn이 전담한다
	//entries 인덱스는 turnOrder 인덱스와 같아야 한다, SetCurrentTurn이 그 전제로 자른다
	TurnScrollBox->ClearChildren();
	entries.Empty();

	for (ACharacterBase* Character : InTurnOrder)
	{
		UTurnOrderEntryWidget* entry = CreateWidget<UTurnOrderEntryWidget>(this, EntryClass);
		if (!entry) continue;

		entry->InitEntry(Character);
		entries.Add(entry);
	}

	SetCurrentTurn(InCurrentIndex);
}

void UTurnOrderWidget::SetCurrentTurn(int32 InCurrentIndex)
{
	if (!TurnScrollBox) return;

	//최상단이 현재 턴, 그 아래로 이번 라운드에 남은 순서만 — 지나간 칸은 목록에서 뺀다
	//다음 라운드는 BuildTurnOrder가 매 라운드 다시 짜고 속도 판정에 난수가 섞이므로
	//미리 붙여 두면 맞지 않는 순서를 보여주게 된다
	TurnScrollBox->ClearChildren();

	for (int32 i = FMath::Max(0, InCurrentIndex); i < entries.Num(); ++i)
	{
		TurnScrollBox->AddChild(entries[i]);

		//재삽입이 칸을 다시 빌드할 수 있으므로 시각 처리는 붙인 뒤에 적용한다
		entries[i]->SetCurrent(i == InCurrentIndex);
	}
}
