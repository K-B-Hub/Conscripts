//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillListEntry.generated.h"

class UButton;
class UImage;
class UTextBlock;
class USkillBase;
class USkillTooltipWidget;

//스킬 목록의 항목 하나, 아이콘과 이름만 표시한다. 액티브·패시브 공용
UCLASS()
class PW_API USkillListEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	//표시 내용 설정, 아이콘 미지정 스킬이면 아이콘을 숨긴다
	//NativeConstruct보다 먼저 불려야 툴팁이 채워진다
	void InitEntry(USkillBase* skill);

protected:
	virtual void NativeConstruct() override;

	//누르는 용도가 아니라 호버를 잡는 히트테스트 표면이다
	//루트가 SelfHitTestInvisible이면 이것 없이는 툴팁이 뜨지 않는다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EntryButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> SkillIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SkillNameText;

	//이 항목의 툴팁으로 달 위젯 클래스, 띄우는 건 Slate가 한다
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USkillTooltipWidget> skillTooltipClass;

private:
	//툴팁에 넘길 스킬, 위젯이 사는 동안 GC로부터 보호한다
	UPROPERTY()
	TObjectPtr<USkillBase> entrySkill = nullptr;
};