//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/DeployMemberEntry.h"
#include "Widget/CharacterTooltipWidget.h"
#include "Run/AllyRunState.h"
#include "Characters/AllyCharacterBase.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UDeployMemberEntry::NativeConstruct()
{
	Super::NativeConstruct();

	if (EntryButton) EntryButton->OnClicked.AddDynamic(this, &UDeployMemberEntry::HandleClicked);
}

void UDeployMemberEntry::InitEntry(const FAllyRunState& state, int32 index)
{
	entryIndex = index;

	if (LevelText) LevelText->SetText(FText::FromString(FString::Printf(TEXT("LV. %d"), state.Level)));

	//이름·직업·초상화·소개는 캐릭터 CDO가 들고 있다, 스냅샷에는 수치만 담긴다
	const AAllyCharacterBase* cdo = AAllyCharacterBase::GetCDO(state.AllyClass);

	if (NameText) NameText->SetText(cdo ? cdo->GetCharacterNameText() : FText::GetEmpty());
	if (JobText)  JobText->SetText(cdo ? cdo->GetJobNameText() : FText::GetEmpty());

	if (JobImage)
	{
		UTexture2D* art = cdo ? cdo->GetPortrait() : nullptr;
		if (art)
		{
			JobImage->SetBrushFromTexture(art);
			JobImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			JobImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	//항목 루트에 달아 둔다, 로스터 수가 바뀔 때만 항목이 다시 만들어지므로 여기서 한 번만 채운다
	if (characterTooltipClass && cdo)
	{
		if (UCharacterTooltipWidget* tooltip = CreateWidget<UCharacterTooltipWidget>(GetOwningPlayer(), characterTooltipClass))
		{
			tooltip->InitTooltip(cdo);
			SetToolTip(tooltip);
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
