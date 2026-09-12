//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/FormationWidget.h"
#include "Widget/JobSelectButton.h"
#include "PlayerController/HubController.h"
#include "GameInstance/PWGameInstance.h"
#include "DataAsset/StoryRouteData.h"
#include "Characters/AllyCharacterBase.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"

void UFormationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConfirmButton) ConfirmButton->OnClicked.AddDynamic(this, &UFormationWidget::HandleConfirmClicked);
	if (BackButton)    BackButton->OnClicked.AddDynamic(this, &UFormationWidget::HandleBackClicked);

	SetupForMode();
	BuildJobList();
	BuildRosterList();
	RefreshConfirmState();
}

void UFormationWidget::SetupForMode()
{
	const UWorld* world = GetWorld();
	const UPWGameInstance* gameInstance = world ? world->GetGameInstance<UPWGameInstance>() : nullptr;
	if (!gameInstance) return;

	const AHubController* hub = GetOwningPlayer<AHubController>();
	const FName routeId = hub ? hub->GetPendingStoryRouteId() : NAME_None;
	const UStoryRouteData* route = gameInstance->FindStoryRoute(routeId);

	//스토리는 줄기의 고정 편성을 그대로 쓰고 편집을 막는다
	if (route)
	{
		bReadOnly = true;

		for (const TSubclassOf<AAllyCharacterBase>& jobClass : route->fixedRoster)
		{
			if (!jobClass) continue;

			pendingRoster.Add(FAllyRunState::MakeFromClass(jobClass, MakeJobLabel(jobClass).ToString()));
		}
		rosterCapacity = pendingRoster.Num();

		if (JobContainer) JobContainer->SetVisibility(ESlateVisibility::Collapsed);
		if (JobHeader)    JobHeader->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	rosterCapacity = gameInstance->GetInitialRosterSize();
}

void UFormationWidget::BuildJobList()
{
	if (bReadOnly || !JobContainer || !jobButtonClass) return;

	const UWorld* world = GetWorld();
	const UPWGameInstance* gameInstance = world ? world->GetGameInstance<UPWGameInstance>() : nullptr;
	if (!gameInstance) return;

	JobContainer->ClearChildren();

	const TArray<TSubclassOf<AAllyCharacterBase>>& jobs = gameInstance->GetSelectableJobs();
	for (int32 i = 0; i < jobs.Num(); ++i)
	{
		if (!jobs[i]) continue;

		UJobSelectButton* entry = CreateWidget<UJobSelectButton>(this, jobButtonClass);
		if (!entry) continue;

		entry->InitEntry(MakeJobLabel(jobs[i]), i);
		entry->OnClicked.BindUObject(this, &UFormationWidget::HandleJobClicked);
		JobContainer->AddChild(entry);
	}
}

void UFormationWidget::BuildRosterList()
{
	if (!RosterContainer || !jobButtonClass) return;

	RosterContainer->ClearChildren();

	for (int32 i = 0; i < pendingRoster.Num(); ++i)
	{
		UJobSelectButton* entry = CreateWidget<UJobSelectButton>(this, jobButtonClass);
		if (!entry) continue;

		entry->InitEntry(FText::FromString(pendingRoster[i].DisplayName), i);

		//읽기 전용이면 표시만 하고 제외 동작을 붙이지 않는다
		if (!bReadOnly)
		{
			entry->OnClicked.BindUObject(this, &UFormationWidget::HandleRosterClicked);
		}
		RosterContainer->AddChild(entry);
	}
}

void UFormationWidget::RefreshConfirmState()
{
	const bool bFilled = pendingRoster.Num() == rosterCapacity && rosterCapacity > 0;

	if (CountText)
	{
		CountText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), pendingRoster.Num(), rosterCapacity)));
	}
	if (ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(bFilled);
	}
}

void UFormationWidget::HandleJobClicked(int32 jobIndex)
{
	if (bReadOnly || pendingRoster.Num() >= rosterCapacity) return;

	const UWorld* world = GetWorld();
	const UPWGameInstance* gameInstance = world ? world->GetGameInstance<UPWGameInstance>() : nullptr;
	if (!gameInstance) return;

	const TArray<TSubclassOf<AAllyCharacterBase>>& jobs = gameInstance->GetSelectableJobs();
	if (!jobs.IsValidIndex(jobIndex) || !jobs[jobIndex]) return;

	//같은 직업을 여러 명 뽑을 수 있으므로 순번을 붙여 개체를 구분한다
	const FString label = FString::Printf(TEXT("%s %d"), *MakeJobLabel(jobs[jobIndex]).ToString(), pendingRoster.Num() + 1);
	pendingRoster.Add(FAllyRunState::MakeFromClass(jobs[jobIndex], label));

	BuildRosterList();
	RefreshConfirmState();
}

void UFormationWidget::HandleRosterClicked(int32 rosterIndex)
{
	if (bReadOnly || !pendingRoster.IsValidIndex(rosterIndex)) return;

	pendingRoster.RemoveAt(rosterIndex);

	BuildRosterList();
	RefreshConfirmState();
}

void UFormationWidget::HandleConfirmClicked()
{
	if (AHubController* hub = GetOwningPlayer<AHubController>())
	{
		hub->ConfirmFormation(pendingRoster);
	}
}

void UFormationWidget::HandleBackClicked()
{
	AHubController* hub = GetOwningPlayer<AHubController>();
	if (!hub) return;

	//스토리는 줄기 선택에서 왔고 나머지는 모드 선택에서 왔다
	if (bReadOnly) hub->ShowStoryRouteSelect();
	else           hub->ShowModeSelect();
}

FText UFormationWidget::MakeJobLabel(TSubclassOf<AAllyCharacterBase> jobClass)
{
	if (!jobClass) return FText::GetEmpty();

	const FText& jobName = jobClass->GetDefaultObject<AAllyCharacterBase>()->jobName;
	return jobName.IsEmpty() ? FText::FromString(jobClass->GetName()) : jobName;
}