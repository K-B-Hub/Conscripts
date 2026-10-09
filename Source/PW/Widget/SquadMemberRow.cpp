//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SquadMemberRow.h"
#include "Characters/CharacterBase.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

namespace
{
	void SetCellInt(UTextBlock* text, int32 value)
	{
		if (text) text->SetText(FText::AsNumber(value));
	}

	void SetCellFloat(UTextBlock* text, float value)
	{
		if (text) text->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), value)));
	}
}

void USquadMemberRow::NativeConstruct()
{
	Super::NativeConstruct();

	if (RowButton) RowButton->OnClicked.AddDynamic(this, &USquadMemberRow::HandleClicked);
}

void USquadMemberRow::InitRow(ACharacterBase* target)
{
	rowTarget = target;
	if (!target) return;

	SetCellInt(LevelText, target->GetLevel());

	if (JobText)
	{
		//직업 이름 미지정 BP면 클래스명으로 대신한다, 상세 정보 창과 같은 폴백
		JobText->SetText(target->jobName.IsEmpty()
			? FText::FromString(target->GetClass()->GetName())
			: target->jobName);
	}

	//체력만 현재/최대 쌍이다, 나머지는 단일 수치
	if (HpText)
	{
		HpText->SetText(FText::FromString(
			FString::Printf(TEXT("%d / %d"), target->GetHp(), target->GetMaxHp())));
	}

	SetCellInt(AtkText, target->GetAtk());
	SetCellInt(DefText, target->GetDef());
	SetCellInt(SpeedText, target->GetSpeed());
	SetCellInt(SkillText, target->GetSkill());

	//명중·회피는 백분율이 아니라 acc-eva 뺄셈식에 들어가는 원시 수치다
	SetCellFloat(AccuracyText, target->GetAccuracy());
	SetCellFloat(EvasionText, target->GetEvasion());
	SetCellFloat(CriticalText, target->GetCritical());
}

void USquadMemberRow::HandleClicked()
{
	OnClicked.ExecuteIfBound(rowTarget);
}
