//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SkillListEntry.h"
#include "Widget/SkillTooltipWidget.h"
#include "Object/Skill/SkillBase.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USkillListEntry::NativeConstruct()
{
	Super::NativeConstruct();

	//항목 루트에 달아 둔다, 스킬이 바뀌지 않으므로 내용은 여기서 한 번만 채운다
	if (skillTooltipClass && entrySkill)
	{
		if (USkillTooltipWidget* tooltip = CreateWidget<USkillTooltipWidget>(GetOwningPlayer(), skillTooltipClass))
		{
			tooltip->InitTooltip(entrySkill);
			SetToolTip(tooltip);
		}
	}
}

void USkillListEntry::InitEntry(USkillBase* skill)
{
	entrySkill = skill;
	if (!skill) return;

	if (SkillNameText)
	{
		//이름이 비어 있으면 클래스명으로 대신한다, 데이터 공백을 빈 칸으로 숨기지 않는다
		SkillNameText->SetText(skill->skillName.IsEmpty()
			? FText::FromString(skill->GetClass()->GetName())
			: skill->skillName);
	}

	if (SkillIcon)
	{
		if (skill->skillIcon)
		{
			SkillIcon->SetBrushFromTexture(skill->skillIcon);
			SkillIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			SkillIcon->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}