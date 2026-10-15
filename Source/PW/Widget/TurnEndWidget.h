// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TurnEndWidget.generated.h"

class UButton;
class ABattleGameMode;
class ABattleController;

//턴 종료 버튼 위젯, BP에서 버튼을 "turnEndButton" 이름으로 배치하면 자동 바인딩됨
UCLASS()
class PW_API UTurnEndWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	//BP의 버튼과 이름으로 자동 바인딩
	//상태 아트와 호버 처리는 UImageButton이 맡는다, 파생도 UButton이라 이 타입으로 받는다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> turnEndButton;
	
	ABattleGameMode* cachedGameMode = nullptr;
	ABattleController* cachedController = nullptr;

	UFUNCTION()
	void OnTurnEndClicked();
};
