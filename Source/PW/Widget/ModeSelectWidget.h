//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ModeSelectWidget.generated.h"

class UButton;

//게임 모드 선택 화면, 잠긴 모드는 버튼을 비활성해 표시
UCLASS()
class PW_API UModeSelectWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StoryButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RoguelikeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NightmareButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

private:
	//세이브의 해금 상태를 버튼 활성 여부에 반영, 잠금 표기는 BP가 IsEnabled에 맞춰 처리
	void ApplyUnlockState();

	UFUNCTION()
	void HandleStoryClicked();
	UFUNCTION()
	void HandleRoguelikeClicked();
	UFUNCTION()
	void HandleNightmareClicked();
	UFUNCTION()
	void HandleBackClicked();
};