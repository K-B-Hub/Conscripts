//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SkillTooltipWidget.h"
#include "Object/Skill/SkillBase.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "Widget/StatusEffectIcon.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USkillTooltipWidget::InitTooltip(const USkillBase* skill)
{
	if (!skill)
	{
		HideTooltip();
		return;
	}

	if (SkillNameText)
	{
		//이름이 비어 있으면 클래스명으로 대신한다, 목록 항목과 같은 폴백
		SkillNameText->SetText(skill->skillName.IsEmpty()
			? FText::FromString(skill->GetClass()->GetName())
			: skill->skillName);
	}

	//효과 설명은 수동 입력 FText라 비어 있을 수 있다, 그때는 빈 줄로 둔다
	if (SkillDescriptionText) SkillDescriptionText->SetText(skill->skillDescription);

	//소모량은 액티브만 갖는다
	const UActiveSkillBase* active = Cast<UActiveSkillBase>(skill);

	if (ActionPointCostText)
	{
		if (active)
		{
			ActionPointCostText->SetText(FText::FromString(FString::Printf(TEXT("AP : %d"), active->GetActionPointCost())));
		}
		ActionPointCostText->SetVisibility(active ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	if (BattleResourceCostText)
	{
		//소모가 없는 스킬에 "0"을 띄우지 않는다
		const bool bHasResourceCost = active && active->battleResourceCost > 0;
		if (bHasResourceCost)
		{
			BattleResourceCostText->SetText(FText::FromString(FString::Printf(TEXT("전투 자원 : %d"), active->battleResourceCost)));
		}
		BattleResourceCostText->SetVisibility(bHasResourceCost ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	if (SkillIcon)
	{
		if (UTexture2D* icon = skill->skillIcon.Get())
		{
			SkillIcon->SetBrushFromTexture(icon);
			SkillIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			SkillIcon->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	//클릭을 먹지 않게 HitTestInvisible로 띄운다, 뒤의 스킬 목록을 계속 누를 수 있어야 한다
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void USkillTooltipWidget::InitStatusTooltip(const FStatusEffectView& view)
{
	if (SkillNameText) SkillNameText->SetText(view.name);
	if (SkillDescriptionText) SkillDescriptionText->SetText(view.description);

	//턴 제한 없는 패시브는 남은 턴을 띄우지 않는다
	if (ActionPointCostText)
	{
		const bool bHasTurn = view.remainingTurn != INDEX_NONE;
		if (bHasTurn)
		{
			ActionPointCostText->SetText(FText::FromString(FString::Printf(TEXT("남은 턴 : %d"), view.remainingTurn)));
		}
		ActionPointCostText->SetVisibility(bHasTurn ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	if (BattleResourceCostText) BattleResourceCostText->SetVisibility(ESlateVisibility::Collapsed);

	if (SkillIcon)
	{
		if (view.icon)
		{
			SkillIcon->SetBrushFromTexture(view.icon);
			SkillIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			SkillIcon->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void USkillTooltipWidget::HideTooltip()
{
	SetVisibility(ESlateVisibility::Collapsed);
}