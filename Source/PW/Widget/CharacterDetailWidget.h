//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterDetailWidget.generated.h"

class ACharacterBase;
class UButton;
class UImage;
class UPanelWidget;
class USkillBase;
class USkillListEntry;
class UTextBlock;

//닫기 요청 통지, 창을 띄운 컨트롤러가 바인딩
DECLARE_DELEGATE(FOnDetailWidgetClosed);

//캐릭터 상세 정보 창, 직업·레벨·스탯·스킬 목록을 표시한다. 아군·적 공통
UCLASS()
class PW_API UCharacterDetailWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//대상의 현재 값을 읽어 표시 내용을 채운다, 창이 열리는 시점에 한 번만 호출(C7)
	void InitDetail(ACharacterBase* target);

	//닫기 통지, 컨트롤러가 창을 거두고 HUD 잠금을 푼다
	FOnDetailWidgetClosed OnClosed;

protected:
	virtual void NativeConstruct() override;

	//스킬 목록에 채울 항목 위젯, WBP에서 지정
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USkillListEntry> skillEntryClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

	//직업 이름·이미지, 미지정이면 각각 클래스명 대체·숨김
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> JobNameText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> JobImage;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LevelText;

	//"현재 / 최대"로 표시하는 자원
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HpText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> BattleResourceText;

	//기본 스탯
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> AtkText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SpeedText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SkillText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DefText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MentalityText;

	//행동 자원, 행동력은 최대치만 표시한다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ActionPointText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MovingPointText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SightText;

	//전투 파생 스탯
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> AccuracyText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EvasionText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CriticalText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PenetrationText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DamageAmplificationText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DamageReductionText;

	//스킬 목록 컨테이너, ScrollBox든 VerticalBox든 받는다
	//ScrollBox로 쓸 경우 부모 슬롯을 Fill로 둘 것 — Automatic이면 높이가 붕괴한다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> ActiveSkillList;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> PassiveSkillList;

private:
	UFUNCTION()
	void HandleCloseClicked();

	//컨테이너를 비우고 스킬마다 항목을 생성해 채운다
	void FillSkillList(UPanelWidget* list, const TArray<USkillBase*>& skills);
};