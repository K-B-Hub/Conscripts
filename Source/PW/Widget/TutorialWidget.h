//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataAsset/TutorialData.h"
#include "TutorialWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;

//튜토리얼 카드, 스크린샷과 설명을 한 장씩 넘겨 보여준다
//루트에 화면을 덮는 Visible 배경이 있어야 뒤쪽 클릭을 막는다
UCLASS()
class PW_API UTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//첫 장부터 표시
	void Open(const UTutorialData* inData);

	//카드를 거두고 GameInstance에 알린다
	UFUNCTION()
	virtual void Close();

protected:
	virtual void NativeConstruct() override;

	//UImage는 클릭을 삼키지 않아 배경을 누르면 월드까지 내려간다
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	//보여줄 장들을 바꾸고 첫 장으로 간다
	void SetPages(const TArray<FTutorialPage>& inPages);

	//마지막 장의 다음 버튼이 닫기로 동작하는지, 끄면 마지막 장에서 비활성이 된다
	bool bCloseOnLastPage = true;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TitleText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> BodyText;

	//스크린샷, ScaleBox(ScaleToFit) 안에 두면 잘라낸 비율 그대로 들어간다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PageImage;

	//스크린샷과 그 틀을 담은 칸, 그림이 없는 장에서는 통째로 접는다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> ImagePanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PrevButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NextButton;

	//NextButton의 라벨, 마지막 장에서 글자가 바뀐다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> NextText;

	//"3 / 9"
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PageText;

	//건너뛰기(X)
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> CloseButton;

private:
	UFUNCTION()
	void HandlePrevClicked();

	UFUNCTION()
	void HandleNextClicked();

	void ShowPage(int32 index);

	//본 기록의 대상, 도움말처럼 기록하지 않는 화면에서는 비어 있다
	UPROPERTY()
	TObjectPtr<const UTutorialData> data = nullptr;

	UPROPERTY()
	TArray<FTutorialPage> pages;

	int32 pageIndex = 0;

	//끝까지 넘겼는지, 본 것으로 칠지 정하는 근거
	bool bReachedLastPage = false;
};
