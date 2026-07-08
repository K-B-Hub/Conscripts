// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/TurnOrderEntryWidget.h"

#include "Components/Image.h"
#include "Characters/CharacterBase.h"

void UTurnOrderEntryWidget::InitEntry(ACharacterBase* InCharacter)
{
	character = InCharacter;
	//TODO: 캐릭터 클래스별 고유 초상화 텍스처를 PortraitImage에 적용
}

void UTurnOrderEntryWidget::SetState(ETurnEntryState InState)
{
	//임시 시각 처리: 지난 턴은 투명한 회색, 현재 턴은 원색, 남은 턴은 회색
	switch (InState)
	{
	case ETurnEntryState::Past:
		SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 0.35f));
		break;
	case ETurnEntryState::Current:
		SetColorAndOpacity(FLinearColor::White);
		break;
	case ETurnEntryState::Upcoming:
		SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 1.f));
		break;
	}

	if (NowMarker)
	{
		NowMarker->SetVisibility(InState == ETurnEntryState::Current
			? ESlateVisibility::HitTestInvisible
			: ESlateVisibility::Collapsed);
	}
}
