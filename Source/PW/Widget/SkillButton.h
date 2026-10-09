// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillButton.generated.h"

class UButton;
class UTextBlock;
class UActiveSkillBase;
class USkillTooltipWidget;

//개별 액티브 스킬에 대응하는 버튼 위젯, 클릭 시 BattleController를 통해 스킬 활성화
UCLASS()
class PW_API USkillButton : public UUserWidget
{
	GENERATED_BODY()

public:
	//대응할 스킬 설정 및 UI 갱신, NativeConstruct보다 먼저 불려야 툴팁이 채워진다
	void InitSkill(UActiveSkillBase* InSkill);

	//CanExecute() 결과에 따라 버튼 활성/비활성 갱신
	void RefreshButtonState();

protected:
	virtual void NativeConstruct() override;

	//BP에서 "SkillButtonElement" 이름으로 바인딩
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SkillButtonElement;

	//BP에서 "SkillNameText" 이름으로 바인딩
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillNameText;

	//이 버튼의 툴팁으로 달 위젯 클래스, 띄우는 건 Slate가 한다
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USkillTooltipWidget> skillTooltipClass;

private:
	UFUNCTION()
	void OnSkillButtonClicked();

	UPROPERTY()
	TObjectPtr<UActiveSkillBase> skill = nullptr;
};