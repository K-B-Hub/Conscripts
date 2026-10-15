//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/BattleHudWidget.h"
#include "Widget/HoverInfoWidget.h"
#include "PlayerController/BattleController.h"
#include "Components/Button.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UBattleHudWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//AddToViewport가 화면 전체를 차지하므로 루트를 그대로 두면 빈 영역이 월드 클릭을 삼킨다
	//전투 내내 떠 있는 위젯이라 이동 명령과 상세보기 대상 선택이 통째로 막힌다
	//Self만 히트 테스트에서 빠지므로 일시정지 버튼·턴 순서 칸은 클릭을 그대로 받는다
	//자식까지 막는 것은 HitTestInvisible이다, 혼동 주의
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SetIsFocusable(false);

	if (PauseButton)
	{
		PauseButton->OnClicked.AddDynamic(this, &UBattleHudWidget::HandlePauseClicked);
	}
}

void UBattleHudWidget::SetHoverTarget(ACharacterBase* target)
{
	if (HoverInfo)
	{
		HoverInfo->ShowFor(target);
	}
}

void UBattleHudWidget::HandlePauseClicked()
{
	if (ABattleController* BC = Cast<ABattleController>(GetOwningPlayer()))
	{
		BC->TogglePauseMenu();
	}

	//클릭 후 포커스를 게임 뷰포트로 반환, 버튼에 포커스가 남아 스페이스바 오입력 방지
	UWidgetBlueprintLibrary::SetFocusToGameViewport();
}