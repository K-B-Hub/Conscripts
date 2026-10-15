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

public:
	//이동력 잔여 여부에 따라 버튼 활성/비활성 갱신
	void RefreshMoveButton(float currentMovingPoint);

protected:
	virtual void NativeConstruct() override;

	//BP에서 버튼을 "MoveButton" 이름으로 배치해야 자동 바인딩됨
	//상태 아트와 호버 처리는 UImageButton이 맡는다, 파생도 UButton이라 이 타입으로 받는다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MoveButton;

private:
	UFUNCTION()
	void OnMoveButtonClicked();
};
