//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/HoverInfoWidget.h"
#include "Widget/StatTextHelpers.h"
#include "Characters/CharacterBase.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Components/ProgressBar.h"

void UHoverInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(false);

	//호버 대상이 생기기 전까지는 자리를 차지하지 않는다
	SetVisibility(ESlateVisibility::Collapsed);
}

void UHoverInfoWidget::ShowFor(ACharacterBase* target)
{
	if (!IsValid(target))
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	//화면 좌측 하단을 덮으므로 월드 클릭을 삼키지 않게 한다
	SetVisibility(ESlateVisibility::HitTestInvisible);

	PWStatText::SetPair(HpText, target->GetHp(), target->GetMaxHp());

	if (JobNameText)
	{
		//직업 이름 미지정 BP면 클래스명으로 대신한다, 상세 정보 창과 같은 폴백
		JobNameText->SetText(target->jobName.IsEmpty()
			? FText::FromString(target->GetClass()->GetName())
			: target->jobName);
	}

	if (CharacterNameText) CharacterNameText->SetText(target->GetCharacterNameText());

	if (LevelText) LevelText->SetText(FText::FromString(FString::Printf(TEXT("LV. %d"), target->GetLevel())));

	if (JobImage)
	{
		if (target->jobIcon)
		{
			JobImage->SetBrushFromTexture(target->jobIcon);
			JobImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			JobImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	PWStatText::SetInt(AtkText, target->GetAtk());
	PWStatText::SetInt(DefText, target->GetDef());

	//accuracy·evasion은 뺄셈식 acc-eva에 들어가는 원시 수치라 단독으로는 %가 아니다
	PWStatText::SetFloat(AccuracyText, target->GetAccuracy());
	PWStatText::SetFloat(EvasionText, target->GetEvasion());
	PWStatText::SetFloat(CriticalText, target->GetCritical());

	//화면 중앙 상단 표시줄, 이름·직업·체력만 간략히 보여준다
	PWStatText::SetPair(TopBarHpText, target->GetHp(), target->GetMaxHp());

	if (TopBarHealthBar)
	{
		//최대 체력이 0 이하인 잘못된 상태면 0으로 나누지 않도록 빈 바로 둔다
		const int32 maxHp = target->GetMaxHp();
		const float ratio = maxHp > 0 ? static_cast<float>(target->GetHp()) / static_cast<float>(maxHp) : 0.f;
		TopBarHealthBar->SetPercent(FMath::Clamp(ratio, 0.f, 1.f));
	}

	//이름·직업 미지정 BP면 클래스명으로 대신한다, 상세 정보 창과 같은 폴백
	if (TopBarNameText) TopBarNameText->SetText(target->GetCharacterNameText());
	if (TopBarJobText)  TopBarJobText->SetText(target->GetJobNameText());
}