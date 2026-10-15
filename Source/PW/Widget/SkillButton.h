// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget/ImageButton.h"
#include "SkillButton.generated.h"

class UImage;
class UTextBlock;
class UActiveSkillBase;
class USkillTooltipWidget;

//슬롯 클릭을 알린다, 스킬 사용과 교환 중 무엇인지는 USkillWidget이 정한다
DECLARE_DELEGATE_OneParam(FOnSkillSlotClicked, int32);

//스킬 슬롯 하나에 대응하는 버튼 위젯, 클릭은 슬롯 인덱스만 올려보낸다
//프레임(기본/호버/클릭)은 UImageButton의 아트가 그리고 스킬 아이콘은 그 위에 얹는다
//빈 슬롯은 아이콘만 숨긴 상태다 — 프레임은 그대로 남아 칸이 비어 보이지 않는다
UCLASS()
class PW_API USkillButton : public UUserWidget
{
	GENERATED_BODY()

public:
	//대응할 스킬 설정 및 UI 갱신, nullptr이면 빈 슬롯이 된다
	//교환이 스킬을 바꾸므로 툴팁도 여기서 매번 다시 만든다
	void InitSkill(UActiveSkillBase* InSkill);

	//이 버튼이 맡은 슬롯 인덱스
	void SetSlotIndex(int32 index) { slotIndex = index; }

	//CanExecute() 결과에 따라 버튼 활성/비활성 갱신, 편집 중에는 전부 켠다
	void RefreshButtonState();

	//편집 모드 진입/해제
	void SetEditMode(bool bEdit);

	//교환 대상으로 집힌 상태 표시
	void SetSelected(bool bInSelected);

	FOnSkillSlotClicked OnSlotClicked;

protected:
	virtual void NativeConstruct() override;

	//BP에서 "SkillButtonElement" 이름으로 바인딩, 프레임 아트와 상태 통지를 담당한다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImageButton> SkillButtonElement;

	//BP에서 "SkillIcon" 이름으로 바인딩, 프레임 위에 얹히므로 버튼 밖 형제다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SkillIcon;

	//BP에서 "SkillNameText" 이름으로 바인딩, 아이콘 버튼에서는 떼도 된다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SkillNameText;

	//이 버튼의 툴팁으로 달 위젯 클래스, 띄우는 건 Slate가 한다
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USkillTooltipWidget> skillTooltipClass;

private:
	UFUNCTION()
	void OnSkillButtonClicked();

	//프레임 상태에 맞춰 아이콘도 교체, 상태 아이콘이 없으면 일반 아이콘을 유지한다
	void ApplyIconForState(EImageButtonState state);

	//선택·사용 가능 여부를 한 곳에서 칠한다, 두 곳에서 칠하면 서로 덮는다
	void RefreshIconTint();

	UPROPERTY()
	TObjectPtr<UActiveSkillBase> skill = nullptr;

	int32 slotIndex = INDEX_NONE;

	bool bEditMode = false;

	//아이콘 틴트를 가르는 두 상태, RefreshIconTint가 둘을 합쳐 칠한다
	bool bSelected = false;
	bool bUsable = true;
};