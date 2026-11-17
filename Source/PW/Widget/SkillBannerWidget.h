//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillBannerWidget.generated.h"

class UTextBlock;
class UImage;
class UWidgetAnimation;
class USoundBase;
class ACharacterBase;
class UActiveSkillBase;
class ABattleGameMode;

//스킬 사용 배너, GameMode의 스킬 알림을 구독해 누가 어떤 스킬을 썼는지 띄운다
//표시 진영은 설정의 스킬 배너 항목을 따른다, 전투 HUD 안에 자식으로 넣는다
UCLASS()
class PW_API USkillBannerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	//시전자 이름, 아군은 고유 이름·적은 직업명
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CasterNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> SkillIcon;

	//등장 연출, 없으면 즉시 표시
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> ShowAnim;

	//배너 등장 효과음
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> announceSound;

	//배너 표시 시간(초), 행동 진행은 기다리지 않는다
	UPROPERTY(EditDefaultsOnly, Category = "SkillBanner", meta = (ClampMin = "0.0"))
	float displayDuration = 2.f;

private:
	void HandleSkillAnnounced(ACharacterBase* caster, const UActiveSkillBase* skill);
	void HideBanner();

	UPROPERTY()
	TObjectPtr<ABattleGameMode> cachedGameMode = nullptr;

	FTimerHandle hideTimer;
};
