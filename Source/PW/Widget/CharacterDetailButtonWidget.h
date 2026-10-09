//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterDetailButtonWidget.generated.h"

class UButton;

//상세보기 요청 통지, 버튼을 띄운 컨트롤러가 바인딩
DECLARE_DELEGATE(FOnDetailButtonClicked);

//캐릭터 옆에 붙는 상세보기 버튼, WidgetComponent에 담겨 화면 공간으로 그려진다
UCLASS()
class PW_API UCharacterDetailButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//클릭 통지, 표시할 때 바인딩하고 거둘 때 해제한다
	FOnDetailButtonClicked OnDetailClicked;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DetailButton;

private:
	UFUNCTION()
	void HandleClicked();
};