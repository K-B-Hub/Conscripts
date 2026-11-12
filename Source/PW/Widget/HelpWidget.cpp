//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/HelpWidget.h"
#include "Widget/HelpTopicButton.h"
#include "DataAsset/HelpMenuData.h"
#include "Components/PanelWidget.h"

void UHelpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//닫기는 X 버튼만, 마지막 장의 다음 버튼은 비활성이 된다
	bCloseOnLastPage = false;

	BuildTopicList();
}

void UHelpWidget::BuildTopicList()
{
	if (!TopicList || !topicButtonClass || !helpData) return;

	//재진입 시 중복 생성 방지
	TopicList->ClearChildren();
	topicButtons.Reset();

	for (int32 i = 0; i < helpData->topics.Num(); ++i)
	{
		const FHelpTopic& topic = helpData->topics[i];

		//장이 없는 항목은 눌러도 보여줄 것이 없다
		if (topic.pages.Num() == 0) continue;

		UHelpTopicButton* button = CreateWidget<UHelpTopicButton>(this, topicButtonClass);
		if (!button) continue;

		button->InitTopic(i, topic.title);
		button->OnClicked.BindUObject(this, &UHelpWidget::SelectTopic);
		TopicList->AddChild(button);
		topicButtons.Add(button);
	}

	if (topicButtons.Num() > 0) SelectTopic(topicButtons[0]->GetTopicIndex());
}

void UHelpWidget::SelectTopic(int32 topicIndex)
{
	if (!helpData || !helpData->topics.IsValidIndex(topicIndex)) return;

	for (UHelpTopicButton* button : topicButtons)
	{
		button->SetSelected(button->GetTopicIndex() == topicIndex);
	}

	SetPages(helpData->topics[topicIndex].pages);
}

void UHelpWidget::Close()
{
	//여는 쪽이 이 위젯을 제거할 수 있으므로 Execute 직후 아무것도 하지 않고 빠진다
	onClosed.ExecuteIfBound();
}
