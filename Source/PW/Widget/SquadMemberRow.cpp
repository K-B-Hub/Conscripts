//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SquadMemberRow.h"
#include "Widget/StatTextHelpers.h"
#include "Widget/CharacterTooltipWidget.h"
#include "Characters/CharacterBase.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void USquadMemberRow::NativeConstruct()
{
	Super::NativeConstruct();

	if (RowButton) RowButton->OnClicked.AddDynamic(this, &USquadMemberRow::HandleClicked);
}

void USquadMemberRow::InitRow(ACharacterBase* target)
{
	rowTarget = target;
	if (!target) return;

	PWStatText::SetInt(LevelText, target->GetLevel());

	//이름·직업 미지정 BP면 클래스명으로 대신한다, 상세 정보 창과 같은 폴백
	if (JobText)  JobText->SetText(target->GetJobNameText());
	if (NameText) NameText->SetText(target->GetCharacterNameText());

	if (PortraitImage)
	{
		if (UTexture2D* art = target->GetPortrait())
		{
			PortraitImage->SetBrushFromTexture(art);
			PortraitImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			PortraitImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	//표가 열릴 때 한 번만 채운다, 행이 쥔 캐릭터는 바뀌지 않는다
	if (characterTooltipClass)
	{
		if (UCharacterTooltipWidget* tooltip = CreateWidget<UCharacterTooltipWidget>(GetOwningPlayer(), characterTooltipClass))
		{
			tooltip->InitTooltip(target);
			SetToolTip(tooltip);
		}
	}

	//체력만 현재/최대 쌍이다, 나머지는 단일 수치
	if (HpText)
	{
		HpText->SetText(FText::FromString(
			FString::Printf(TEXT("%d / %d"), target->GetHp(), target->GetMaxHp())));
	}

	PWStatText::SetInt(AtkText, target->GetAtk());
	PWStatText::SetInt(DefText, target->GetDef());
	PWStatText::SetInt(SpeedText, target->GetSpeed());
	PWStatText::SetInt(SkillText, target->GetSkill());

	//명중·회피는 백분율이 아니라 acc-eva 뺄셈식에 들어가는 원시 수치다
	PWStatText::SetFloat(AccuracyText, target->GetAccuracy());
	PWStatText::SetFloat(EvasionText, target->GetEvasion());
	PWStatText::SetFloat(CriticalText, target->GetCritical());
}

void USquadMemberRow::HandleClicked()
{
	OnClicked.ExecuteIfBound(rowTarget);
}
