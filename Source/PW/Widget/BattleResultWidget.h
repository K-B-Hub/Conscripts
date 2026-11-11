//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameMode/BattleGameMode.h"
#include "BattleResultWidget.generated.h"

class UButton;
class UPanelWidget;
class UTextBlock;
class UWidget;
class UResultMemberEntry;
class AAllyCharacterBase;

//전투 결과 화면, 승리면 다음 스테이지로 패배면 메인메뉴로 보낸다
//승리와 패배는 레이아웃이 달라 패널 두 벌을 두고 한쪽만 켠다, 배경도 각 패널이 들고 있다
UCLASS()
class PW_API UBattleResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//결과를 받아 표시 내용을 구성, 컨트롤러가 생성 직후 호출
	void SetResult(EBattleResult result);

protected:
	virtual void NativeConstruct() override;

	//승리 패널, 전적 요약과 생존자 목록이 들어간다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> VictoryPanel;

	//패배 패널, 반투명 배경 위에 문구와 메인메뉴 버튼만 둔다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> DefeatPanel;

	//"승리" / "패배"
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ResultText;

	//생존자 목록, 레벨과 체력이 이월되었는지 눈으로 확인하는 용도
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> SurvivorContainer;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> ContinueButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ContinueLabel;

	//패배 시 메인메뉴로, 두 버튼 모두 LeaveBattle로 가며 분기는 결과가 한다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> MainMenuButton;

	//생존자 항목 위젯 클래스, BP에서 지정
	//결과 화면 전용이라 편성·증원 목록과 아트를 따로 입힐 수 있다
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UResultMemberEntry> survivorEntryClass;

private:
	void BuildSurvivorList();

	//생존자 항목 클릭, 그 캐릭터의 상세 정보 창을 띄운다
	//같은 캐릭터를 두 명 둘 수 없으므로 클래스로 전장의 액터를 특정할 수 있다
	void HandleSurvivorClicked(TSubclassOf<AAllyCharacterBase> characterClass);

	UFUNCTION()
	void HandleContinueClicked();

	EBattleResult battleResult = EBattleResult::Victory;
};