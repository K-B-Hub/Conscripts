//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/TurnBannerWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Characters/AllyCharacterBase.h"
#include "GameMode/BattleGameMode.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

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

void UTurnBannerWidget::HandleTurnAnnounced(ACharacterBase* turnUnit)
{
	if (!turnUnit || displayDuration <= 0.f) return;

	const AAllyCharacterBase* ally = Cast<AAllyCharacterBase>(turnUnit);

	if (TurnText)
	{
		TurnText->SetText(FText::FromString(ally ? TEXT("아군 턴") : TEXT("적 턴")));
	}
	if (RoundText && cachedGameMode)
	{
		//라운드 증가가 알림보다 먼저라 새 라운드 첫 턴도 갱신된 값을 읽는다
		RoundText->SetText(FText::FromString(FString::Printf(TEXT("라운드 %d"), cachedGameMode->GetCurrentRound())));
	}
	if (UnitNameText)
	{
		//아군은 고유 이름, 적은 고유 이름을 지정하지 않으므로 직업명
		UnitNameText->SetText(ally ? ally->GetCharacterNameText() : turnUnit->GetJobNameText());
	}
	if (JobNameText)
	{
		//적은 이름 칸에 이미 직업명이 들어가 있어 같은 글자를 두 번 띄우지 않는다
		JobNameText->SetText(turnUnit->GetJobNameText());
		JobNameText->SetVisibility(ally ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	//턴은 이미 시작됐으므로 클릭을 막지 않는다
	SetVisibility(ESlateVisibility::HitTestInvisible);
	if (ShowAnim) PlayAnimation(ShowAnim);
	if (turnStartSound) UGameplayStatics::PlaySound2D(this, turnStartSound);

	GetWorld()->GetTimerManager().SetTimer(hideTimer, this, &UTurnBannerWidget::HideBanner, displayDuration, false);
}

void UTurnBannerWidget::HideBanner()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
