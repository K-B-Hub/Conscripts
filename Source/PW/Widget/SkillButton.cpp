// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SkillButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "PlayerController/BattleController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void USkillButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (SkillButtonElement)
	{
		SkillButtonElement->OnClicked.AddDynamic(this, &USkillButton::OnSkillButtonClicked);
	}
}

void USkillButton::InitSkill(UActiveSkillBase* InSkill)
{
	skill = InSkill;

	if (SkillNameText && skill)
	{
		SkillNameText->SetText(skill->skillName);
	}
}

void USkillButton::RefreshButtonState()
{
	if (!SkillButtonElement || !skill) return;

	SkillButtonElement->SetIsEnabled(skill->CanExecute());
}

void USkillButton::OnSkillButtonClicked()
{
	if (!skill) return;

	if (ABattleController* BC = Cast<ABattleController>(GetOwningPlayer()))
	{
		BC->ActivateSkill(skill);
	}

	// 클릭 후 포커스를 게임 뷰포트로 반환
	UWidgetBlueprintLibrary::SetFocusToGameViewport();
}