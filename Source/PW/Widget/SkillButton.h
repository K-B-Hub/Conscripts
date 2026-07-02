// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillButton.generated.h"

class UButton;
class UTextBlock;
class UActiveSkillBase;

//개별 액티브 스킬에 대응하는 버튼 위젯, 클릭 시 BattleController를 통해 스킬 활성화
UCLASS()
class PW_API USkillButton : public UUserWidget
{
	GENERATED_BODY()

public:
	//대응할 스킬 설정 및 UI 갱신
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

private:
	UFUNCTION()
	void OnSkillButtonClicked();

	UPROPERTY()
	TObjectPtr<UActiveSkillBase> skill = nullptr;
};