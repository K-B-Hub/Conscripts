// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TurnEndWidget.generated.h"

class UButton;
class ABattleGameMode;
class ABattleController;

// 턴 종료 버튼 위젯.
// BP에서 이름이 "TurnEndButton"인 버튼을 만들면 자동 바인딩됨.
UCLASS()
class PW_API UTurnEndWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	// BP의 버튼과 이름으로 자동 바인딩
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> turnEndButton;
	
	ABattleGameMode* cachedGameMode = nullptr;
	ABattleController* cachedController = nullptr;

	UFUNCTION()
	void OnTurnEndClicked();
};
