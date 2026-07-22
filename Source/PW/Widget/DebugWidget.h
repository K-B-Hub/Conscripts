// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DebugWidget.generated.h"

class UButton;

//디버그 조작 위젯, 현재 턴 캐릭터의 스트레스/레벨을 즉시 조작
UCLASS()
class PW_API UDebugWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	//스트레스 +100
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AddStressButton;

	//레벨 +1
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LevelUpButton;

private:
	UFUNCTION()
	void OnAddStressClicked();

	UFUNCTION()
	void OnLevelUpClicked();
};
