//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/CampOptionWidget.h"
#include "Widget/CampOptionButton.h"
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

		UCampOptionButton* entry = CreateWidget<UCampOptionButton>(this, optionButtonClass);
		if (!entry) continue;

		entry->InitEntry(options[i], i);
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