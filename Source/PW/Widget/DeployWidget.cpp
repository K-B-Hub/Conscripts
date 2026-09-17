//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/DeployWidget.h"
#include "Widget/JobSelectButton.h"
#include "PlayerController/BattleController.h"
#include "GameMode/BattleGameMode.h"
#include "GameInstance/PWGameInstance.h"
#include "Run/RunProgress.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"

void UDeployWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//UUserWidget의 기본 Visibility는 Visible이고 AddToViewport는 화면 전체를 차지한다
	//그대로 두면 빈 영역이 월드 클릭을 삼켜 배치가 불가능해진다. 자식 버튼은 영향받지 않는다
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SetIsFocusable(false);

	if (ConfirmButton) ConfirmButton->OnClicked.AddDynamic(this, &UDeployWidget::HandleConfirmClicked);

	RefreshList();
}

void UDeployWidget::RefreshList()
{
	if (!MemberContainer || !memberButtonClass) return;

	const UWorld* world = GetWorld();
	if (!world) return;

	ABattleGameMode* gameMode = world->GetAuthGameMode<ABattleGameMode>();
	const UPWGameInstance* gameInstance = world->GetGameInstance<UPWGameInstance>();
	const URunProgress* runProgress = gameInstance ? gameInstance->GetRunProgress() : nullptr;
	if (!gameMode || !runProgress) return;

	MemberContainer->ClearChildren();

	const TArray<FAllyRunState>& roster = runProgress->GetRoster();
	for (int32 i = 0; i < roster.Num(); ++i)
	{
		UJobSelectButton* entry = CreateWidget<UJobSelectButton>(this, memberButtonClass);
		if (!entry) continue;

		//항목 위젯이 텍스트 한 줄만 받으므로 선택·배치 상태를 문구로 표시한다
		FString label = roster[i].DisplayName;
		if (gameMode->IsDeployed(i)) label += TEXT(" (배치됨)");
		if (i == selectedIndex)      label = TEXT("▶ ") + label;

		entry->InitEntry(FText::FromString(label), i);
		entry->OnClicked.BindUObject(this, &UDeployWidget::HandleMemberClicked);
		MemberContainer->AddChild(entry);
	}

	if (ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(gameMode->IsDeploymentComplete());
	}

	if (HintText)
	{
		const FString hint = roster.IsValidIndex(selectedIndex)
			? FString::Printf(TEXT("%s 배치할 위치를 클릭하세요"), *roster[selectedIndex].DisplayName)
			: TEXT("배치할 인원을 선택하세요");
		HintText->SetText(FText::FromString(hint));
	}
}

void UDeployWidget::HandleMemberClicked(int32 rosterIndex)
{
	//같은 항목을 다시 누르면 선택 해제
	selectedIndex = (selectedIndex == rosterIndex) ? INDEX_NONE : rosterIndex;

	RefreshList();
}

void UDeployWidget::HandleConfirmClicked()
{
	//위젯 제거까지 컨트롤러가 처리하므로 GameMode를 직접 부르지 않는다
	if (ABattleController* battleController = GetOwningPlayer<ABattleController>())
	{
		battleController->ConfirmDeployment();
	}
}