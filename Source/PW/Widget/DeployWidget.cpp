//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/DeployWidget.h"
#include "Widget/DeployMemberEntry.h"
#include "PlayerController/BattleController.h"
#include "GameMode/BattleGameMode.h"
#include "GameInstance/PWGameInstance.h"
#include "Run/RunProgress.h"
#include "Characters/AllyCharacterBase.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UDeployWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//UUserWidget의 기본 Visibility는 Visible이고 AddToViewport는 화면 전체를 차지한다
	//그대로 두면 빈 영역이 월드 클릭을 삼켜 배치가 불가능해진다. 자식 버튼은 영향받지 않는다
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SetIsFocusable(false);

	if (ConfirmButton) ConfirmButton->OnClicked.AddDynamic(this, &UDeployWidget::HandleConfirmClicked);
	if (ResetButton)   ResetButton->OnClicked.AddDynamic(this, &UDeployWidget::HandleResetClicked);

	RefreshList();
}

void UDeployWidget::RefreshList()
{
	if (!MemberContainer || !memberEntryClass) return;

	const UWorld* world = GetWorld();
	if (!world) return;

	ABattleGameMode* gameMode = world->GetAuthGameMode<ABattleGameMode>();
	const UPWGameInstance* gameInstance = world->GetGameInstance<UPWGameInstance>();
	const URunProgress* runProgress = gameInstance ? gameInstance->GetRunProgress() : nullptr;
	if (!gameMode || !runProgress) return;

	const TArray<FAllyRunState>& roster = runProgress->GetRoster();

	//로스터가 줄어 선택이 범위를 벗어났으면 해제한다
	if (!roster.IsValidIndex(selectedIndex)) selectedIndex = INDEX_NONE;

	if (memberEntries.Num() != roster.Num()) RebuildEntries(roster);

	int32 deployedCount = 0;
	for (int32 i = 0; i < memberEntries.Num(); ++i)
	{
		const bool bDeployed = gameMode->IsDeployed(i);
		if (bDeployed) ++deployedCount;

		if (memberEntries[i]) memberEntries[i]->SetEntryState(i == selectedIndex, bDeployed);
	}

	const bool bComplete = gameMode->IsDeploymentComplete();

	if (ConfirmButton) ConfirmButton->SetIsEnabled(bComplete);
	if (ResetButton)   ResetButton->SetIsEnabled(deployedCount > 0);

	if (DeployCountText)
	{
		DeployCountText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), deployedCount, roster.Num())));
	}

	if (DeployCompletePanel)
	{
		DeployCompletePanel->SetVisibility(bComplete ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	if (HintText)
	{
		FString hint;
		if (roster.IsValidIndex(selectedIndex))
		{
			hint = TEXT("배치할 위치를 클릭하세요");
		}
		else
		{
			hint = bComplete ? TEXT("출격 준비가 끝났습니다") : TEXT("배치할 인원을 선택하세요");
		}
		HintText->SetText(FText::FromString(hint));
	}

	RefreshInfoPanel(roster);
}

void UDeployWidget::RebuildEntries(const TArray<FAllyRunState>& roster)
{
	MemberContainer->ClearChildren();
	memberEntries.Reset();

	for (int32 i = 0; i < roster.Num(); ++i)
	{
		//생성에 실패해도 자리를 채워 로스터 인덱스와 항목 인덱스를 어긋나지 않게 한다
		UDeployMemberEntry* entry = CreateWidget<UDeployMemberEntry>(this, memberEntryClass);
		memberEntries.Add(entry);
		if (!entry) continue;

		entry->InitEntry(roster[i], GetJobIcon(roster[i].AllyClass), i);
		entry->OnClicked.BindUObject(this, &UDeployWidget::HandleMemberClicked);
		MemberContainer->AddChild(entry);
	}
}

void UDeployWidget::RefreshInfoPanel(const TArray<FAllyRunState>& roster)
{
	if (!roster.IsValidIndex(selectedIndex))
	{
		if (InfoPanel) InfoPanel->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	const FAllyRunState& state = roster[selectedIndex];

	if (InfoPanel) InfoPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (InfoJobImage)
	{
		if (UTexture2D* icon = GetJobIcon(state.AllyClass))
		{
			InfoJobImage->SetBrushFromTexture(icon);
			InfoJobImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			InfoJobImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (InfoNameText)  InfoNameText->SetText(FText::FromString(state.DisplayName));
	if (InfoJobText)   InfoJobText->SetText(GetJobName(state.AllyClass));
	if (InfoLevelText) InfoLevelText->SetText(FText::FromString(FString::Printf(TEXT("LV. %d"), state.Level)));

	if (InfoHpText)    InfoHpText->SetText(FText::AsNumber(state.MaxHp));
	if (InfoAtkText)   InfoAtkText->SetText(FText::AsNumber(state.Atk));
	if (InfoDefText)   InfoDefText->SetText(FText::AsNumber(state.Def));
	if (InfoSpeedText) InfoSpeedText->SetText(FText::AsNumber(state.Speed));
	if (InfoSkillText) InfoSkillText->SetText(FText::AsNumber(state.Skill));
	if (InfoApText)    InfoApText->SetText(FText::AsNumber(state.ActionPoint));
}

FText UDeployWidget::GetJobName(TSubclassOf<AAllyCharacterBase> jobClass)
{
	if (!jobClass) return FText::GetEmpty();

	const AAllyCharacterBase* jobCDO = jobClass->GetDefaultObject<AAllyCharacterBase>();
	if (!jobCDO || jobCDO->jobName.IsEmpty()) return FText::FromString(jobClass->GetName());

	return jobCDO->jobName;
}

UTexture2D* UDeployWidget::GetJobIcon(TSubclassOf<AAllyCharacterBase> jobClass)
{
	if (!jobClass) return nullptr;

	const AAllyCharacterBase* jobCDO = jobClass->GetDefaultObject<AAllyCharacterBase>();
	return jobCDO ? jobCDO->jobIcon.Get() : nullptr;
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

void UDeployWidget::HandleResetClicked()
{
	selectedIndex = INDEX_NONE;

	//목록 갱신까지 컨트롤러가 처리한다
	if (ABattleController* battleController = GetOwningPlayer<ABattleController>())
	{
		battleController->ResetDeployment();
	}
}
