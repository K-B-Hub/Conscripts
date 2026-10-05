//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/DeployMemberEntry.h"
#include "Run/AllyRunState.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UDeployMemberEntry::NativeConstruct()
{
	Super::NativeConstruct();

	if (EntryButton) EntryButton->OnClicked.AddDynamic(this, &UDeployMemberEntry::HandleClicked);
}

void UDeployMemberEntry::InitEntry(const FAllyRunState& state, UTexture2D* jobIcon, int32 index)
{
	entryIndex = index;

	if (NameText)  NameText->SetText(FText::FromString(state.DisplayName));
	if (LevelText) LevelText->SetText(FText::FromString(FString::Printf(TEXT("LV. %d"), state.Level)));

	if (JobImage)
	{
		if (jobIcon)
		{
			JobImage->SetBrushFromTexture(jobIcon);
			JobImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			JobImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	//최대 체력이 0 이하인 잘못된 스냅샷이면 0으로 나누지 않도록 빈 바로 둔다
	if (HpBar)
	{
		const float ratio = state.MaxHp > 0 ? static_cast<float>(state.Hp) / static_cast<float>(state.MaxHp) : 0.f;
		HpBar->SetPercent(FMath::Clamp(ratio, 0.f, 1.f));
	}
	if (HpText) HpText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), state.Hp, state.MaxHp)));
}

void UDeployMemberEntry::SetEntryState(bool bSelected, bool bDeployed)
{
	if (SelectedHighlight) SelectedHighlight->SetVisibility(bSelected ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	if (DeployedMark)      DeployedMark->SetVisibility(bDeployed ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}

void UDeployMemberEntry::HandleClicked()
{
	OnClicked.ExecuteIfBound(entryIndex);
}
