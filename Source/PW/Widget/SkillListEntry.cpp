//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SkillListEntry.h"
#include "Widget/SkillTooltipWidget.h"
#include "Object/Skill/SkillBase.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

namespace
{
	//브러시에 텍스처와 네이티브 크기를 함께 넣는다
	//ImageSize를 안 바꾸면 이전 아트의 크기로 늘어나 그려진다
	void SetBrushTexture(FSlateBrush& brush, UTexture2D* texture)
	{
		if (!texture) return;

		brush.SetResourceObject(texture);
		brush.ImageSize = FVector2D(texture->GetSizeX(), texture->GetSizeY());
	}
}

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

void USkillListEntry::InitEntry(USkillBase* skill, int32 count)
{
	entrySkill = skill;
	if (!skill) return;

	ApplyGradeArt(skill->skillGrade);

	if (SkillNameText)
	{
		//이름이 비어 있으면 클래스명으로 대신한다, 데이터 공백을 빈 칸으로 숨기지 않는다
		const FText name = skill->skillName.IsEmpty()
			? FText::FromString(skill->GetClass()->GetName())
			: skill->skillName;

		//1개면 곱하기를 붙이지 않는다, 읽을 필요 없는 "x 1"이 항상 따라붙는다
		SkillNameText->SetText(count >= 2
			? FText::FromString(FString::Printf(TEXT("%s x %d"), *name.ToString(), count))
			: name);
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

void USkillListEntry::ApplyGradeArt(EUpgradeGrade grade)
{
	if (!EntryButton) return;

	const FSkillEntryGradeArt* art = gradeArt.Find(grade);
	if (!art) return;

	FButtonStyle style = EntryButton->GetStyle();
	SetBrushTexture(style.Normal, art->normal);
	SetBrushTexture(style.Hovered, art->hovered);
	//클릭은 기능이 없으므로 눌러도 호버 모양을 유지한다
	SetBrushTexture(style.Pressed, art->hovered);
	EntryButton->SetStyle(style);
}