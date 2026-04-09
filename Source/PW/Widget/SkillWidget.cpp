// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SkillWidget.h"
#include "Widget/SkillButton.h"
#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "Components/VerticalBox.h"

void USkillWidget::InitSkills(USkillComponent* SkillComp)
{
	if (!SkillComp || !SkillButtonContainer || !skillButtonClass) return;

	SkillButtonContainer->ClearChildren();
	skillButtons.Empty();

	TArray<UActiveSkillBase*> ActiveSkills = SkillComp->GetActiveSkills();
	for (UActiveSkillBase* Skill : ActiveSkills)
	{
		USkillButton* Button = CreateWidget<USkillButton>(GetOwningPlayer(), skillButtonClass);
		if (Button)
		{
			Button->InitSkill(Skill);
			SkillButtonContainer->AddChildToVerticalBox(Button);
			skillButtons.Add(Button);
		}
	}
}

void USkillWidget::RefreshButtons()
{
	for (USkillButton* Button : skillButtons)
	{
		if (IsValid(Button))
		{
			Button->RefreshButtonState();
		}
	}
}