//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SquadMemberRow.generated.h"

class UButton;
class UTextBlock;
class ACharacterBase;

//행 클릭 시 자신이 쥔 캐릭터를 통지, 부대 정보표가 상세 정보 창을 띄우는 데 쓴다
DECLARE_DELEGATE_OneParam(FOnSquadRowClicked, ACharacterBase*);

//부대 정보표의 한 줄, 칸 순서는 표의 헤더 라벨과 같아야 한다
UCLASS()
class PW_API USquadMemberRow : public UUserWidget
{
	GENERATED_BODY()

public:
	//대상의 현재 값을 읽어 칸을 채운다, 표가 열리는 시점에 한 번만 호출
	void InitRow(ACharacterBase* target);

	//클릭 통지, 표를 만든 쪽이 바인딩
	FOnSquadRowClicked OnClicked;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RowButton;

	//칸 10개, 전부 선택 바인딩이라 이름이 틀려도 컴파일은 통과하고 조용히 빈 칸이 된다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LevelText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> JobText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HpText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> AtkText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DefText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SpeedText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SkillText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> AccuracyText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EvasionText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CriticalText;

private:
	UFUNCTION()
	void HandleClicked();

	//통지할 대상, 위젯이 사는 동안 GC로부터 보호한다
	UPROPERTY()
	TObjectPtr<ACharacterBase> rowTarget = nullptr;
};
