//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;

//메인메뉴 화면, 버튼 처리를 HubController로 곧장 넘긴다
UCLASS()
class PW_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartButton;
	//저장된 런이 없으면 비활성, 편성을 건너뛰고 그 스테이지로 바로 들어간다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SettingsButton;
	//WBP에 버튼을 놓기 전에도 컴파일되도록 옵션
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> HelpButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;

private:
	UFUNCTION()
	void HandleStartClicked();
	UFUNCTION()
	void HandleContinueClicked();
	UFUNCTION()
	void HandleSettingsClicked();
	UFUNCTION()
	void HandleHelpClicked();
	UFUNCTION()
	void HandleQuitClicked();
};