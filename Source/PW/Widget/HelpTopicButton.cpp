//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/HelpTopicButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UHelpTopicButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (TopicButton) TopicButton->OnClicked.AddDynamic(this, &UHelpTopicButton::HandleClicked);
}

void UHelpTopicButton::InitTopic(int32 inTopicIndex, const FText& title)
{
	topicIndex = inTopicIndex;

	if (TitleText) TitleText->SetText(title);
}

void UHelpTopicButton::SetSelected(bool bSelected)
{
	if (SelectedMark)
	{
		SelectedMark->SetVisibility(bSelected ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
}

void UHelpTopicButton::HandleClicked()
{
	OnClicked.ExecuteIfBound(topicIndex);
}
