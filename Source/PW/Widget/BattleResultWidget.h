//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameMode/BattleGameMode.h"
#include "BattleResultWidget.generated.h"

class UButton;
class UPanelWidget;
class UTextBlock;
class UJobSelectButton;

//전투 결과 화면, 승리면 다음 스테이지로 패배면 메인메뉴로 보낸다
UCLASS()
class PW_API UBattleResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//결과를 받아 표시 내용을 구성, 컨트롤러가 생성 직후 호출
	void SetResult(EBattleResult result);

protected:
	virtual void NativeConstruct() override;

	//"승리" / "패배"
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultText;

	//생존자 목록, 레벨과 체력이 이월되었는지 눈으로 확인하는 용도
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> SurvivorContainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ContinueLabel;

	//생존자 항목 위젯 클래스, BP에서 지정
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UJobSelectButton> survivorEntryClass;

private:
	void BuildSurvivorList();

	UFUNCTION()
	void HandleContinueClicked();

	EBattleResult battleResult = EBattleResult::Victory;
};