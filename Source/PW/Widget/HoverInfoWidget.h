//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HoverInfoWidget.generated.h"

class ACharacterBase;
class UImage;
class UProgressBar;
class UTextBlock;

//호버한 캐릭터의 간단 정보, 화면 좌측 하단에 고정된다
//커서 아래 캐릭터와 턴 순서 초상화가 같은 위젯을 쓴다
UCLASS()
class PW_API UHoverInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//대상의 현재 값으로 채우고 표시한다, nullptr이면 숨긴다
	void ShowFor(ACharacterBase* target);

protected:
	virtual void NativeConstruct() override;

	//체력은 바가 아니라 "7 / 10" 숫자로만 보여준다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HpText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> JobNameText;
	//캐릭터 고유 이름, 직업명과 별개 칸이다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CharacterNameText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LevelText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> JobImage;

	//기본 스탯
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> AtkText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DefText;

	//전투 파생 스탯, 상세 정보 창과 달리 명중·회피·치명 셋만 보여준다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> AccuracyText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EvasionText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CriticalText;

	//화면 중앙 상단 대상 표시줄, 위 스탯 칸들과 같은 호버 대상을 다른 자리에서 보여준다
	//위 HpText와 체력을 중복 표시하는 것이 맞다 — 둘은 화면상 다른 위치의 칸이다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> TopBarHealthBar;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TopBarHpText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TopBarNameText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TopBarJobText;
};