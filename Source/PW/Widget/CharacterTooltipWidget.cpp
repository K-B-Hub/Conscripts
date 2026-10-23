//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/CharacterTooltipWidget.h"
#include "Characters/CharacterBase.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UCharacterTooltipWidget::InitTooltip(const ACharacterBase* character)
{
	if (!character) return;

	if (CharacterNameText) CharacterNameText->SetText(character->GetCharacterNameText());
	if (JobNameText)       JobNameText->SetText(character->GetJobNameText());

	//소개는 수동 입력 FText라 비어 있을 수 있다, 그때는 자리를 접는다
	if (DescriptionText)
	{
		DescriptionText->SetText(character->characterDescription);
		DescriptionText->SetVisibility(character->characterDescription.IsEmpty()
			? ESlateVisibility::Collapsed
			: ESlateVisibility::HitTestInvisible);
	}

	//고유 초상화가 없으면 직업 아이콘이 나온다, 둘 다 없으면 빈 사각형 대신 숨긴다
	if (PortraitImage)
	{
		if (UTexture2D* art = character->GetPortrait())
		{
			PortraitImage->SetBrushFromTexture(art);
			PortraitImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			PortraitImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
