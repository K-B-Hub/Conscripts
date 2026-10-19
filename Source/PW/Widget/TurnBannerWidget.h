//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TurnBannerWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;
class ACharacterBase;
class ABattleGameMode;

//턴 전환 배너, GameMode의 턴 알림을 구독해 지연 동안만 누구 차례인지 띄운다
//전투 HUD 안에 자식으로 넣는다, HUD가 첫 턴 알림 전에 생성되므로 첫 턴부터 받는다
UCLASS()
class PW_API UTurnBannerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	//"아군 턴"/"적 턴"
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TurnText;

	//턴 주인 이름, 아군은 개체 이름·적은 직업명
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> UnitNameText;

	//등장 연출, 없으면 즉시 표시
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> ShowAnim;

private:
	void HandleTurnAnnounced(ACharacterBase* turnUnit, float duration);
	void HideBanner();

	UPROPERTY()
	TObjectPtr<ABattleGameMode> cachedGameMode = nullptr;

	FTimerHandle hideTimer;
};
