// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SkillButton.h"
#include "Widget/SkillTooltipWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void USkillButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (SkillButtonElement)
	{
		SkillButtonElement->OnClicked.AddDynamic(this, &USkillButton::OnSkillButtonClicked);

		//아이콘은 버튼 밖 형제라 상태 전환이 닿지 않는다, 프레임의 상태를 받아 같이 바꾼다
		SkillButtonElement->OnStateChanged.BindUObject(this, &USkillButton::ApplyIconForState);
	}
}

void USkillButton::InitSkill(UActiveSkillBase* InSkill)
{
	skill = InSkill;

	if (SkillNameText)
	{
		SkillNameText->SetText(skill ? skill->skillName : FText::GetEmpty());
	}

	//스킬이 바뀌면 지금 상태의 아이콘으로 다시 그린다
	ApplyIconForState(SkillButtonElement ? SkillButtonElement->GetButtonState() : EImageButtonState::Normal);

	//버튼 루트에 달아 둔다, 내부 UButton이 비활성이어도 툴팁은 뜬다
	//교환으로 스킬이 바뀌므로 NativeConstruct가 아니라 여기서 매번 다시 만든다
	if (skill && skillTooltipClass)
	{
		if (USkillTooltipWidget* tooltip = CreateWidget<USkillTooltipWidget>(GetOwningPlayer(), skillTooltipClass))
		{
			tooltip->InitTooltip(skill);
			SetToolTip(tooltip);
		}
	}
	else
	{
		SetToolTip(nullptr);
	}
}

void USkillButton::ApplyIconForState(EImageButtonState state)
{
	if (!SkillIcon) return;

	UTexture2D* icon = nullptr;
	if (skill)
	{
		//상태 아이콘이 비어 있으면 일반 아이콘으로 떨어진다, 한 장만 꽂아도 동작한다
		if (state == EImageButtonState::Hovered) icon = skill->skillIconHovered.Get();
		else if (state == EImageButtonState::Pressed) icon = skill->skillIconPressed.Get();

		if (!icon) icon = skill->skillIcon.Get();
	}

	//빈 슬롯과 아이콘 미지정 스킬은 아이콘만 감춘다, 프레임 아트가 남아 칸이 비어 보이지 않는다
	if (icon)
	{
		SkillIcon->SetBrushFromTexture(icon);
		SkillIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		SkillIcon->SetVisibility(ESlateVisibility::Hidden);
	}
}

void USkillButton::RefreshButtonState()
{
	if (!SkillButtonElement) return;

	//편집 중에는 쓸 수 없는 스킬과 빈 칸도 집어야 하므로 전부 켠다
	bUsable = bEditMode || (skill && skill->CanExecute());
	SkillButtonElement->SetIsEnabled(bUsable);

	RefreshIconTint();
}

void USkillButton::RefreshIconTint()
{
	if (!SkillIcon) return;

	//아이콘은 버튼 밖 형제라 버튼의 비활성 처리가 닿지 않는다, 여기서 한 곳으로 모아 칠한다
	//교환 선택이 비활성 표시를 덮는다, 편집 중에는 쓸 수 없는 스킬도 집히기 때문이다
	FLinearColor tint = FLinearColor::White;
	if (bSelected) tint = FLinearColor(0.45f, 0.45f, 0.45f, 1.f);
	else if (!bUsable) tint = FLinearColor(0.4f, 0.4f, 0.4f, 1.f);

	SkillIcon->SetColorAndOpacity(tint);
}

void USkillButton::SetEditMode(bool bEdit)
{
	bEditMode = bEdit;
	RefreshButtonState();
}

void USkillButton::SetSelected(bool bInSelected)
{
	//집힌 칸을 어둡게 해 둔다, 아트가 들어오면 전용 프레임으로 바꿀 자리다
	bSelected = bInSelected;
	RefreshIconTint();
}

void USkillButton::OnSkillButtonClicked()
{
	OnSlotClicked.ExecuteIfBound(slotIndex);

	//클릭 후 포커스를 게임 뷰포트로 반환
	UWidgetBlueprintLibrary::SetFocusToGameViewport();
}