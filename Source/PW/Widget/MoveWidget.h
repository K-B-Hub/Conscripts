// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MoveWidget.generated.h"

class UButton;

UCLASS()
class PW_API UMoveWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// BP에서 버튼 이름을 "MoveButton"으로 지정해야 자동 바인딩됨
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MoveButton;

private:
	UFUNCTION()
	void OnMoveButtonClicked();
};
