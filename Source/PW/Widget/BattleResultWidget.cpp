//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/BattleResultWidget.h"
#include "Widget/JobSelectButton.h"
#include "PlayerController/BattleController.h"
#include "GameInstance/PWGameInstance.h"
#include "Run/RunProgress.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"

void UBattleResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (ContinueButton) ContinueButton->OnClicked.AddDynamic(this, &UBattleResultWidget::HandleContinueClicked);
}

void UBattleResultWidget::SetResult(EBattleResult result)
{
	battleResult = result;

	const bool bVictory = result == EBattleResult::Victory;

	if (ResultText)
	{
		ResultText->SetText(FText::FromString(bVictory ? TEXT("승리") : TEXT("패배")));
	}
	if (ContinueLabel)
	{
		ContinueLabel->SetText(FText::FromString(bVictory ? TEXT("계속") : TEXT("메인메뉴로")));
	}

	//패배하면 로스터가 비어 있어 보여줄 생존자가 없다
	if (bVictory) BuildSurvivorList();
}

void UBattleResultWidget::BuildSurvivorList()
{
	if (!SurvivorContainer || !survivorEntryClass) return;

	const UWorld* world = GetWorld();
	const UPWGameInstance* gameInstance = world ? world->GetGameInstance<UPWGameInstance>() : nullptr;
	const URunProgress* runProgress = gameInstance ? gameInstance->GetRunProgress() : nullptr;
	if (!runProgress) return;

	SurvivorContainer->ClearChildren();

	//이 시점의 로스터는 이미 CaptureFromWorld로 갱신되어 생존자만 남아 있다
	const TArray<FAllyRunState>& roster = runProgress->GetRoster();
	for (int32 i = 0; i < roster.Num(); ++i)
	{
		UJobSelectButton* entry = CreateWidget<UJobSelectButton>(this, survivorEntryClass);
		if (!entry) continue;

		const FString label = FString::Printf(TEXT("%s  Lv.%d  HP %d/%d"),
			*roster[i].DisplayName, roster[i].Level, roster[i].Hp, roster[i].MaxHp);

		entry->InitEntry(FText::FromString(label), i);
		SurvivorContainer->AddChild(entry);
	}
}

void UBattleResultWidget::HandleContinueClicked()
{
	if (ABattleController* battleController = GetOwningPlayer<ABattleController>())
	{
		battleController->LeaveBattle(battleResult);
	}
}