//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/TurnBannerWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Characters/AllyCharacterBase.h"
#include "GameMode/BattleGameMode.h"
#include "TimerManager.h"

void UTurnBannerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);

	cachedGameMode = Cast<ABattleGameMode>(GetWorld()->GetAuthGameMode());
	if (cachedGameMode)
	{
		cachedGameMode->OnTurnAnnounced.AddUObject(this, &UTurnBannerWidget::HandleTurnAnnounced);
	}
}

void UTurnBannerWidget::NativeDestruct()
{
	if (cachedGameMode)
	{
		cachedGameMode->OnTurnAnnounced.RemoveAll(this);
		cachedGameMode = nullptr;
	}
	if (UWorld* world = GetWorld()) world->GetTimerManager().ClearTimer(hideTimer);

	Super::NativeDestruct();
}

void UTurnBannerWidget::HandleTurnAnnounced(ACharacterBase* turnUnit, float duration)
{
	if (!turnUnit || duration <= 0.f) return;

	const AAllyCharacterBase* ally = Cast<AAllyCharacterBase>(turnUnit);

	if (TurnText)
	{
		TurnText->SetText(FText::FromString(ally ? TEXT("아군 턴") : TEXT("적 턴")));
	}
	if (UnitNameText)
	{
		//아군은 개체 이름, 적은 개체 이름이 없어 직업명
		UnitNameText->SetText(ally ? FText::FromString(ally->displayName) : turnUnit->jobName);
	}

	//클릭을 막지 않는다, 지연 중 조작 차단은 activeUnit이 비어 있는 것으로 충분하다
	SetVisibility(ESlateVisibility::HitTestInvisible);
	if (ShowAnim) PlayAnimation(ShowAnim);

	GetWorld()->GetTimerManager().SetTimer(hideTimer, this, &UTurnBannerWidget::HideBanner, duration, false);
}

void UTurnBannerWidget::HideBanner()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
