//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/CampRecruitWidget.h"
#include "Widget/JobSelectButton.h"
#include "Characters/AllyCharacterBase.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"

void UCampRecruitWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (ConfirmButton) ConfirmButton->OnClicked.AddDynamic(this, &UCampRecruitWidget::HandleConfirmClicked);

	//후보 목록은 Open이 채운다, 띄우는 쪽만 누가 남았는지 안다
}

void UCampRecruitWidget::Open(const TArray<TSubclassOf<AAllyCharacterBase>>& candidates, int32 count)
{
	candidatePool.Reset();
	for (const TSubclassOf<AAllyCharacterBase>& characterClass : candidates)
	{
		if (characterClass) candidatePool.AddUnique(characterClass);
	}

	pendingRecruits.Reset();

	//후보가 인원보다 적으면 채울 수 없는 화면에 갇히므로 정원을 후보 수로 줄인다
	recruitCapacity = FMath::Min(count, candidatePool.Num());

	BuildJobList();
	BuildRosterList();
	RefreshConfirmState();
}

void UCampRecruitWidget::BuildJobList()
{
	if (!JobContainer || !jobButtonClass) return;

	JobContainer->ClearChildren();

	for (const TSubclassOf<AAllyCharacterBase>& characterClass : candidatePool)
	{
		//합류 목록에 넣은 캐릭터는 후보에서 빼 중복 선택 자체를 없앤다
		if (pendingRecruits.Contains(characterClass)) continue;

		UJobSelectButton* entry = CreateWidget<UJobSelectButton>(this, jobButtonClass);
		if (!entry) continue;

		entry->InitEntry(characterClass);
		entry->OnClicked.BindUObject(this, &UCampRecruitWidget::HandleCharacterClicked);
		JobContainer->AddChild(entry);
	}
}

void UCampRecruitWidget::BuildRosterList()
{
	if (!RosterContainer || !jobButtonClass) return;

	RosterContainer->ClearChildren();

	for (const TSubclassOf<AAllyCharacterBase>& characterClass : pendingRecruits)
	{
		UJobSelectButton* entry = CreateWidget<UJobSelectButton>(this, jobButtonClass);
		if (!entry) continue;

		entry->InitEntry(characterClass);
		entry->OnClicked.BindUObject(this, &UCampRecruitWidget::HandleRosterClicked);
		RosterContainer->AddChild(entry);
	}
}

void UCampRecruitWidget::RefreshConfirmState()
{
	const bool bFilled = pendingRecruits.Num() == recruitCapacity && recruitCapacity > 0;

	if (CountText)
	{
		CountText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), pendingRecruits.Num(), recruitCapacity)));
	}
	if (ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(bFilled);
	}
}

void UCampRecruitWidget::HandleCharacterClicked(TSubclassOf<AAllyCharacterBase> characterClass)
{
	if (pendingRecruits.Num() >= recruitCapacity) return;
	if (!candidatePool.Contains(characterClass) || pendingRecruits.Contains(characterClass)) return;

	pendingRecruits.Add(characterClass);

	BuildJobList();
	BuildRosterList();
	RefreshConfirmState();
}

void UCampRecruitWidget::HandleRosterClicked(TSubclassOf<AAllyCharacterBase> characterClass)
{
	if (pendingRecruits.Remove(characterClass) == 0) return;

	BuildJobList();
	BuildRosterList();
	RefreshConfirmState();
}

void UCampRecruitWidget::HandleConfirmClicked()
{
	OnRecruitConfirmed.ExecuteIfBound(pendingRecruits);
}
