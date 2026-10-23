//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/JobSelectButton.h"
#include "Widget/CharacterTooltipWidget.h"
#include "Characters/AllyCharacterBase.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UJobSelectButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (JobButton) JobButton->OnClicked.AddDynamic(this, &UJobSelectButton::HandleClicked);
}

void UJobSelectButton::InitEntry(TSubclassOf<AAllyCharacterBase> characterClass)
{
	entryCharacterClass = characterClass;

	const AAllyCharacterBase* cdo = AAllyCharacterBase::GetCDO(characterClass);
	if (!cdo) return;

	if (LabelText) LabelText->SetText(cdo->GetCharacterNameText());
	if (JobText)   JobText->SetText(cdo->GetJobNameText());

	//덧붙일 정보가 들어오기 전까지는 자리를 접어 둔다
	if (DetailText) DetailText->SetVisibility(ESlateVisibility::Collapsed);

	if (PortraitImage)
	{
		if (UTexture2D* art = cdo->GetPortrait())
		{
			PortraitImage->SetBrushFromTexture(art);
			PortraitImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			PortraitImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	//항목 루트에 달아 둔다, 캐릭터가 바뀌지 않으므로 내용은 여기서 한 번만 채운다
	if (characterTooltipClass)
	{
		if (UCharacterTooltipWidget* tooltip = CreateWidget<UCharacterTooltipWidget>(GetOwningPlayer(), characterTooltipClass))
		{
			tooltip->InitTooltip(cdo);
			SetToolTip(tooltip);
		}
	}
}

void UJobSelectButton::SetDetailText(const FText& detail)
{
	if (!DetailText) return;

	DetailText->SetText(detail);
	DetailText->SetVisibility(detail.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
}

void UJobSelectButton::HandleClicked()
{
	OnClicked.ExecuteIfBound(entryCharacterClass);
}
