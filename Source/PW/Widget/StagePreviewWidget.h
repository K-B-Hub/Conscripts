//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Run/StageEntry.h"
#include "StagePreviewWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;
class UTexture2D;

//결과 화면 다음에 오는 다음 스테이지 예고
//추첨은 이미 끝난 뒤라 여기서는 정해진 결과를 보여주기만 한다
//악몽은 야영지 버튼으로 이 자리에 야영지를 끼워 넣을 수 있다
UCLASS()
class PW_API UStagePreviewWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	//"남은 스테이지 9" 같은 잔여 표시
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ProgressText;

	//다음 스테이지, 야영지면 야영지 아이콘과 이름을 쓴다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> NextIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> NextNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> NextDescText;

	//그 다음 스테이지, 런의 마지막이면 아이콘을 숨기고 종료라고 표시한다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> FollowingIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> FollowingNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NextButton;

	//야영지 방문권이 없는 모드에서는 통째로 숨긴다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> CampButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CampCountText;

	//야영지 스테이지는 임무 데이터가 없어 아이콘을 여기서 받는다
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TObjectPtr<UTexture2D> campIcon;

private:
	//현재 예고 내용으로 표시를 다시 채운다, 야영지를 끼워 넣은 뒤에도 호출된다
	void Refresh();

	//스테이지 한 칸의 아이콘, 없으면 이미지를 숨긴다
	void ApplyStageIcon(UImage* image, const FStageEntry* stage) const;

	//스테이지 한 칸의 이름과 설명
	static FText DescribeStageName(const FStageEntry* stage);
	static FText DescribeStageDetail(const FStageEntry* stage);

	UFUNCTION()
	void HandleNextClicked();

	UFUNCTION()
	void HandleCampClicked();
};
