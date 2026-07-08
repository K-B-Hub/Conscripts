// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TurnEndWidget.generated.h"

class UCircularButtonWidget;
class ABattleGameMode;
class ABattleController;

//턴 종료 버튼 위젯, BP에서 CircularButton 파생 BP를 "turnEndButton" 이름으로 배치하면 자동 바인딩됨
UCLASS()
class PW_API UTurnEndWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	//BP의 버튼과 이름으로 자동 바인딩
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCircularButtonWidget> turnEndButton;
	
	ABattleGameMode* cachedGameMode = nullptr;
	ABattleController* cachedController = nullptr;

	UFUNCTION()
	void OnTurnEndClicked();
};
