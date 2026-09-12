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
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SettingsButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;

private:
	UFUNCTION()
	void HandleStartClicked();
	UFUNCTION()
	void HandleSettingsClicked();
	UFUNCTION()
	void HandleQuitClicked();
};