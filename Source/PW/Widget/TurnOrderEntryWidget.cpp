// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/TurnOrderEntryWidget.h"

#include "Components/Image.h"
#include "Widget/ImageButton.h"
#include "Characters/CharacterBase.h"
#include "Engine/Texture2D.h"
#include "PlayerController/BattleController.h"

void UTurnOrderEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//UUserWidget 기본값은 SelfHitTestInvisible이라 칸 자체가 히트 대상이 아니다
	//TeamFrame 버튼이 칸 대부분을 덮지만 남는 여백의 클릭도 삼켜야 하므로 루트를 열어 둔다
	//멱등한 설정이므로 칸 재삽입으로 여기가 다시 돌아도 안전하다(T5)
	SetVisibility(ESlateVisibility::Visible);

	if (TeamFrame)
	{
		TeamFrame->OnClicked.AddUniqueDynamic(this, &UTurnOrderEntryWidget::HandleFrameClicked);
	}
}

void UTurnOrderEntryWidget::NativeDestruct()
{
	//칸이 목록에서 빠질 때는 MouseLeave가 오지 않는다, 내가 쥔 호버는 내가 거둔다
	if (ABattleController* BC = Cast<ABattleController>(GetOwningPlayer()))
	{
		BC->ClearHoverFromWidget(character);
	}

	Super::NativeDestruct();
}

void UTurnOrderEntryWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (ABattleController* BC = Cast<ABattleController>(GetOwningPlayer()))
	{
		BC->SetHoverFromWidget(character);
	}
}

void UTurnOrderEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (ABattleController* BC = Cast<ABattleController>(GetOwningPlayer()))
	{
		BC->ClearHoverFromWidget(character);
	}
}

FReply UTurnOrderEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton) return FReply::Unhandled();

	//버튼이 받은 클릭은 여기까지 올라오지 않는다, 여기 오는 것은 버튼 밖 여백뿐이다
	if (ABattleController* BC = Cast<ABattleController>(GetOwningPlayer()))
	{
		BC->FocusCameraOn(character);
	}

	//캐릭터가 없어도 삼킨다, 칸이 가린 자리의 클릭이 월드 이동 명령으로 새면 안 된다
	return FReply::Handled();
}

void UTurnOrderEntryWidget::HandleFrameClicked()
{
	if (ABattleController* BC = Cast<ABattleController>(GetOwningPlayer()))
	{
		BC->FocusCameraOn(character);
	}
}

void UTurnOrderEntryWidget::InitEntry(ACharacterBase* InCharacter)
{
	character = InCharacter;
	if (!character) return;

	//고유 초상화가 없으면 직업 아이콘, 둘 다 없으면 빈 사각형이 남는 대신 숨긴다
	if (PortraitImage)
	{
		if (UTexture2D* art = character->GetPortrait())
		{
			PortraitImage->SetBrushFromTexture(art);
			PortraitImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			PortraitImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	ApplyFrameArt();
}

void UTurnOrderEntryWidget::SetCurrent(bool bIsCurrent)
{
	bCurrentTurn = bIsCurrent;

	//진영·턴 상태는 프레임 아트가 구분한다, 여기서 틴트를 걸면 그 아트를 덮는다
	ApplyFrameArt();

	if (NowMarker)
	{
		NowMarker->SetVisibility(bIsCurrent
			? ESlateVisibility::HitTestInvisible
			: ESlateVisibility::Collapsed);
	}
}

void UTurnOrderEntryWidget::ApplyFrameArt()
{
	if (!TeamFrame || !character) return;

	const bool bAlly = character->IsAlly();

	//아군·적이 한 목록에 섞이므로 진영과 턴 상태를 네 세트로 구분한다
	if (bAlly)
	{
		TeamFrame->SetStateTextures(bCurrentTurn ? allyCurrentArt : allyIdleArt);
	}
	else
	{
		TeamFrame->SetStateTextures(bCurrentTurn ? enemyCurrentArt : enemyIdleArt);
	}
}