// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enum/UpgradeGrade.h"
#include "UpgradeSelectWidget.generated.h"

class UUpgradeChoiceButton;
class USkillBase;
class UButton;
class UPanelWidget;
class UWidget;

//강화 선택이 완료되면 선택된 스킬 클래스를 통지
DECLARE_DELEGATE_OneParam(FOnUpgradeChosen, TSubclassOf<USkillBase>);

//레벨업 강화 선택 위젯, 최대 3개 후보 버튼을 표시하고 선택 결과를 통지
UCLASS()
class PW_API UUpgradeSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//후보 목록으로 선택지 구성, 초과 슬롯은 숨김
	//grade로 보일 컨테이너를 고른다, 아트는 그 컨테이너에 든 파생 버튼이 들고 있다
	void SetChoices(const TArray<TSubclassOf<USkillBase>>& Choices, EUpgradeGrade grade);

	//선택 완료 통지, BattleController가 바인딩
	FOnUpgradeChosen OnUpgradeChosen;

protected:
	virtual void NativeConstruct() override;

	//등급별 선택지 컨테이너, 각자 그 등급 아트를 입힌 파생 버튼 3개를 미리 담고 있다
	//한 번에 하나만 보이고 나머지는 Collapsed다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> LowChoices;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> MidChoices;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> HighChoices;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> TopChoices;

	//등급 컨테이너 네 개를 묶는 상위 컨테이너, 접기/펴기로 아군·적 상황 확인
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> ChoicesContainer;

	//선택지 표시를 토글하는 버튼, 접어도 선택은 유지
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ToggleButton;

private:
	void HandleChoiceClicked(TSubclassOf<USkillBase> Chosen);

	//등급에 대응하는 컨테이너, 미지정 등급은 Low로 떨어진다
	UPanelWidget* ContainerForGrade(EUpgradeGrade grade) const;

	UFUNCTION()
	void HandleToggleClicked();

	//선택지 컨테이너 현재 표시 여부
	bool bChoicesVisible = true;
};
