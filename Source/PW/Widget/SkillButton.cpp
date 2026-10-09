// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SkillButton.h"
#include "Widget/SkillTooltipWidget.h"
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

	//버튼 루트에 달아 둔다, 내부 UButton이 비활성이어도 툴팁은 뜬다
	//스킬이 바뀌지 않으므로 내용은 여기서 한 번만 채운다
	if (skillTooltipClass && skill)
	{
		if (USkillTooltipWidget* tooltip = CreateWidget<USkillTooltipWidget>(GetOwningPlayer(), skillTooltipClass))
		{
			tooltip->InitTooltip(skill);
			SetToolTip(tooltip);
		}
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

	//클릭 후 포커스를 게임 뷰포트로 반환
	UWidgetBlueprintLibrary::SetFocusToGameViewport();
}