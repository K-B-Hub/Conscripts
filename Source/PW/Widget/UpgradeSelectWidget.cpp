// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/UpgradeSelectWidget.h"
#include "Widget/UpgradeChoiceButton.h"
#include "Components/Button.h"

void UUpgradeSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetIsFocusable(false);

	if (ToggleButton)
	{
		ToggleButton->OnClicked.AddDynamic(this, &UUpgradeSelectWidget::HandleToggleClicked);
	}
}

void UUpgradeSelectWidget::HandleToggleClicked()
{
	bChoicesVisible = !bChoicesVisible;
	if (ChoicesContainer)
	{
		//접으면 선택지만 숨기고, 위젯 자체는 남아 재표시 가능
		ChoicesContainer->SetVisibility(bChoicesVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UUpgradeSelectWidget::SetChoices(const TArray<TSubclassOf<USkillBase>>& Choices)
{
	UUpgradeChoiceButton* buttons[3] = { Choice0, Choice1, Choice2 };
	for (int32 i = 0; i < 3; ++i)
	{
		if (!buttons[i]) continue;

		if (Choices.IsValidIndex(i))
		{
			buttons[i]->InitChoice(Choices[i]);
			buttons[i]->OnClicked.BindUObject(this, &UUpgradeSelectWidget::HandleChoiceClicked);
			buttons[i]->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			//후보보다 슬롯이 많으면 남는 슬롯 숨김
			buttons[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UUpgradeSelectWidget::HandleChoiceClicked(TSubclassOf<USkillBase> Chosen)
{
	OnUpgradeChosen.ExecuteIfBound(Chosen);
}
