//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsWidget.generated.h"

class UButton;

//설정 화면, 현재는 복귀만 가능한 껍데기
//해상도·음량·조작키 항목은 전체 흐름 완성 후 추가
UCLASS()
class PW_API USettingsWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

private:
	UFUNCTION()
	void HandleBackClicked();
};