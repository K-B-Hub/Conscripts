//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SquadMemberRow.h"
#include "Widget/StatTextHelpers.h"
#include "Characters/CharacterBase.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

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
