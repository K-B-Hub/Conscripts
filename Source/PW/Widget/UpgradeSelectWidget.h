// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeSelectWidget.generated.h"

class UUpgradeChoiceButton;
class USkillBase;
class UButton;
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
	void SetChoices(const TArray<TSubclassOf<USkillBase>>& Choices);

	//선택 완료 통지, BattleController가 바인딩
	FOnUpgradeChosen OnUpgradeChosen;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUpgradeChoiceButton> Choice0;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUpgradeChoiceButton> Choice1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUpgradeChoiceButton> Choice2;

	//선택지 3개를 담는 컨테이너, 접기/펴기로 아군·적 상황 확인
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> ChoicesContainer;

	//선택지 표시를 토글하는 버튼, 접어도 선택은 유지
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ToggleButton;

private:
	void HandleChoiceClicked(TSubclassOf<USkillBase> Chosen);

	UFUNCTION()
	void HandleToggleClicked();

	//선택지 컨테이너 현재 표시 여부
	bool bChoicesVisible = true;
};
