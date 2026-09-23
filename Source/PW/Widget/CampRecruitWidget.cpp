//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/CampRecruitWidget.h"
#include "Widget/JobSelectButton.h"
#include "GameInstance/PWGameInstance.h"
#include "Characters/AllyCharacterBase.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"

void UCampRecruitWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	BuildJobList();
}

void UCampRecruitWidget::BuildJobList()
{
	if (!JobContainer || !jobButtonClass) return;

	const UWorld* world = GetWorld();
	const UPWGameInstance* gameInstance = world ? world->GetGameInstance<UPWGameInstance>() : nullptr;
	if (!gameInstance) return;

	JobContainer->ClearChildren();

	//편성과 같은 목록이라 인덱스가 그대로 통한다
	const TArray<TSubclassOf<AAllyCharacterBase>> jobs = gameInstance->GetUnlockedJobs();
	for (int32 i = 0; i < jobs.Num(); ++i)
	{
		if (!jobs[i]) continue;

		UJobSelectButton* entry = CreateWidget<UJobSelectButton>(this, jobButtonClass);
		if (!entry) continue;

		const FText& jobName = jobs[i]->GetDefaultObject<AAllyCharacterBase>()->jobName;
		entry->InitEntry(jobName.IsEmpty() ? FText::FromString(jobs[i]->GetName()) : jobName, i);
		entry->OnClicked.BindUObject(this, &UCampRecruitWidget::HandleJobClicked);
		JobContainer->AddChild(entry);
	}
}

void UCampRecruitWidget::SetRemaining(int32 remaining)
{
	if (RemainingText)
	{
		RemainingText->SetText(FText::FromString(FString::Printf(TEXT("합류시킬 동료 %d명"), remaining)));
	}
}

void UCampRecruitWidget::HandleJobClicked(int32 jobIndex)
{
	OnJobChosen.ExecuteIfBound(jobIndex);
}