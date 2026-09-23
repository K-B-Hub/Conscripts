//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CampOptionWidget.generated.h"

class UButton;
class UPanelWidget;
class UJobSelectButton;

//야영지 정비 선택 화면, 메인 캠프를 클릭하면 열린다
UCLASS()
class PW_API UCampOptionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	//선택지 버튼이 붙을 컨테이너
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> OptionContainer;

	//선택하지 않고 닫기
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

	//선택지 항목 위젯 클래스, BP에서 지정
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UJobSelectButton> optionButtonClass;

private:
	void BuildOptionList();

	void HandleOptionClicked(int32 optionIndex);

	UFUNCTION()
	void HandleCloseClicked();
};