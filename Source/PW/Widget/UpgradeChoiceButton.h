// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeChoiceButton.generated.h"

class UButton;
class UTextBlock;
class USkillBase;

//강화 후보 하나를 클릭하면 대응 스킬 클래스를 통지
DECLARE_DELEGATE_OneParam(FOnUpgradeChoiceClicked, TSubclassOf<USkillBase>);

//강화 선택지 하나에 대응하는 버튼 위젯, 이름/설명은 스킬 CDO에서 표시
UCLASS()
class PW_API UUpgradeChoiceButton : public UUserWidget
{
	GENERATED_BODY()

public:
	//표시할 강화 후보 설정
	void InitChoice(TSubclassOf<USkillBase> InSkillClass);

	//클릭 통지, 상위 선택 위젯이 바인딩
	FOnUpgradeChoiceClicked OnClicked;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ChoiceButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DescText;

private:
	UFUNCTION()
	void HandleClicked();

	UPROPERTY()
	TSubclassOf<USkillBase> skillClass;
};
