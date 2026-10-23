//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/CampRecruitWidget.h"
#include "Widget/JobSelectButton.h"
#include "Characters/AllyCharacterBase.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"

void UCampRecruitWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	//후보 목록은 SetCandidates가 채운다, 띄우는 쪽만 누가 남았는지 안다
}

void UCampRecruitWidget::SetCandidates(const TArray<TSubclassOf<AAllyCharacterBase>>& candidates)
{
	if (!JobContainer || !jobButtonClass) return;

	JobContainer->ClearChildren();

	for (const TSubclassOf<AAllyCharacterBase>& characterClass : candidates)
	{
		if (!characterClass) continue;

		UJobSelectButton* entry = CreateWidget<UJobSelectButton>(this, jobButtonClass);
		if (!entry) continue;

		entry->InitEntry(characterClass);
		entry->OnClicked.BindUObject(this, &UCampRecruitWidget::HandleCharacterClicked);
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

void UCampRecruitWidget::HandleCharacterClicked(TSubclassOf<AAllyCharacterBase> characterClass)
{
	OnCharacterChosen.ExecuteIfBound(characterClass);
}
