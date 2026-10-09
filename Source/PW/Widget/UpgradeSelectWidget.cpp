// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/UpgradeSelectWidget.h"
#include "Widget/UpgradeChoiceButton.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"

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

void UUpgradeSelectWidget::SetChoices(const TArray<TSubclassOf<USkillBase>>& Choices, EUpgradeGrade grade)
{
	UPanelWidget* const containers[] = { LowChoices, MidChoices, HighChoices, TopChoices };
	UPanelWidget* const active = ContainerForGrade(grade);

	for (UPanelWidget* container : containers)
	{
		if (!container) continue;
		container->SetVisibility(container == active ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (!active) return;

	for (int32 i = 0; i < active->GetChildrenCount(); ++i)
	{
		UUpgradeChoiceButton* button = Cast<UUpgradeChoiceButton>(active->GetChildAt(i));
		if (!button) continue;

		if (Choices.IsValidIndex(i))
		{
			button->InitChoice(Choices[i]);
			button->OnClicked.BindUObject(this, &UUpgradeSelectWidget::HandleChoiceClicked);
			button->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			//후보보다 슬롯이 많으면 남는 슬롯 숨김
			button->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

UPanelWidget* UUpgradeSelectWidget::ContainerForGrade(EUpgradeGrade grade) const
{
	switch (grade)
	{
	case EUpgradeGrade::Mid:  return MidChoices;
	case EUpgradeGrade::High: return HighChoices;
	case EUpgradeGrade::Top:  return TopChoices;
	default:                  return LowChoices;
	}
}

void UUpgradeSelectWidget::HandleChoiceClicked(TSubclassOf<USkillBase> Chosen)
{
	OnUpgradeChosen.ExecuteIfBound(Chosen);
}
