//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/CampOptionButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "DataAsset/CampOptionData.h"

void UCampOptionButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (JobButton) JobButton->OnClicked.AddDynamic(this, &UCampOptionButton::HandleClicked);
}

void UCampOptionButton::InitEntry(const UCampOptionData* option, int32 index)
{
	entryIndex = index;

	if (!option) return;

	if (TitleText) TitleText->SetText(option->Title);
	if (DescText) DescText->SetText(option->Description);

	//아이콘 미지정이면 이미지를 숨겨 빈 브러시가 보이지 않게
	if (IconImage)
	{
		IconImage->SetBrushFromTexture(option->Icon);
		IconImage->SetVisibility(option->Icon ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
}

void UCampOptionButton::HandleClicked()
{
	OnClicked.ExecuteIfBound(entryIndex);
}
