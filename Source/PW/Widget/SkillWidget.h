// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillWidget.generated.h"

class UVerticalBox;
class USkillButton;
class USkillComponent;

// 턴 시작 시 생성되어 액티브 스킬 버튼을 동적으로 배치하는 위젯
UCLASS()
class PW_API USkillWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// SkillComponent의 액티브 스킬 목록으로 버튼 생성
	void InitSkills(USkillComponent* SkillComp);

protected:
	// BP에서 "SkillButtonContainer" 이름의 VerticalBox와 자동 바인딩
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> SkillButtonContainer;

	// 스킬 버튼 위젯 클래스 (BP에서 WBP_SkillButton 지정)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USkillButton> skillButtonClass;
};