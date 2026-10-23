//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/BattleResultWidget.h"
#include "Widget/ResultMemberEntry.h"
#include "PlayerController/BattleController.h"
#include "GameInstance/PWGameInstance.h"
#include "Characters/AllyCharacterBase.h"
#include "Run/RunProgress.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"

void UBattleResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (ContinueButton) ContinueButton->OnClicked.AddDynamic(this, &UBattleResultWidget::HandleContinueClicked);

	//메인메뉴로 보낼지 다음 스테이지로 보낼지는 LeaveBattle이 결과로 가리므로 핸들러를 공유한다
	if (MainMenuButton) MainMenuButton->OnClicked.AddDynamic(this, &UBattleResultWidget::HandleContinueClicked);
}

void UBattleResultWidget::SetResult(EBattleResult result)
{
	battleResult = result;

	const bool bVictory = result == EBattleResult::Victory;

	//한쪽만 켠다, 접힌 패널은 자리도 차지하지 않아 반대쪽 배경이 비치지 않는다
	if (VictoryPanel) VictoryPanel->SetVisibility(bVictory ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (DefeatPanel)  DefeatPanel->SetVisibility(bVictory ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

	if (ResultText)
	{
		ResultText->SetText(FText::FromString(bVictory ? TEXT("VICTORY!") : TEXT("GAME OVER")));
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
		UResultMemberEntry* entry = CreateWidget<UResultMemberEntry>(this, survivorEntryClass);
		if (!entry) continue;

		//이름·직업·초상화는 캐릭터 CDO가 채우고, 레벨과 체력은 이 전투의 결과다
		entry->InitEntry(roster[i]);
		entry->OnClicked.BindUObject(this, &UBattleResultWidget::HandleSurvivorClicked);

		SurvivorContainer->AddChild(entry);
	}
}

void UBattleResultWidget::HandleSurvivorClicked(TSubclassOf<AAllyCharacterBase> characterClass)
{
	ABattleController* battleController = GetOwningPlayer<ABattleController>();
	ABattleGameMode* gameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!battleController || !gameMode || !characterClass) return;

	//전투가 끝나도 생존자 액터는 레벨 전환 전까지 전장에 남아 있다
	//사망자는 이미 배열에서 빠졌으므로 여기서 찾히는 것은 모두 살아 돌아온 캐릭터다
	for (const TObjectPtr<AAllyCharacterBase>& ally : gameMode->GetAllies())
	{
		if (ally && ally->GetClass() == characterClass)
		{
			battleController->OpenDetailWidget(ally);
			return;
		}
	}
}

void UBattleResultWidget::HandleContinueClicked()
{
	if (ABattleController* battleController = GetOwningPlayer<ABattleController>())
	{
		battleController->LeaveBattle(battleResult);
	}
}