//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/CampOptionWidget.h"
#include "Widget/JobSelectButton.h"
#include "PlayerController/CampController.h"
#include "GameMode/CampGameMode.h"
#include "DataAsset/CampOptionData.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"

void UCampOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//빈 영역이 야영지 클릭을 삼키지 않도록
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (CloseButton) CloseButton->OnClicked.AddDynamic(this, &UCampOptionWidget::HandleCloseClicked);

	BuildOptionList();
}

void UCampOptionWidget::BuildOptionList()
{
	if (!OptionContainer || !optionButtonClass) return;

	const UWorld* world = GetWorld();
	const ACampGameMode* camp = world ? world->GetAuthGameMode<ACampGameMode>() : nullptr;
	if (!camp) return;

	OptionContainer->ClearChildren();

	const TArray<TObjectPtr<UCampOptionData>>& options = camp->GetCampOptions();
	for (int32 i = 0; i < options.Num(); ++i)
	{
		if (!options[i]) continue;

		UJobSelectButton* entry = CreateWidget<UJobSelectButton>(this, optionButtonClass);
		if (!entry) continue;

		//항목 위젯이 텍스트 한 줄만 받으므로 제목과 설명을 붙여 보여준다
		const FString label = FString::Printf(TEXT("%s — %s"),
			*options[i]->Title.ToString(), *options[i]->Description.ToString());

		entry->InitEntry(FText::FromString(label), i);
		entry->OnClicked.BindUObject(this, &UCampOptionWidget::HandleOptionClicked);
		OptionContainer->AddChild(entry);
	}
}

void UCampOptionWidget::HandleOptionClicked(int32 optionIndex)
{
	if (ACampController* campController = GetOwningPlayer<ACampController>())
	{
		campController->ChooseCampOption(optionIndex);
	}
}

void UCampOptionWidget::HandleCloseClicked()
{
	if (ACampController* campController = GetOwningPlayer<ACampController>())
	{
		campController->CloseCampOptions();
	}
}