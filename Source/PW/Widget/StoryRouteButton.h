//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoryRouteButton.generated.h"

class UButton;
class UTextBlock;
class UStoryRouteData;

//줄기 항목을 클릭하면 대응 줄기 ID를 통지
DECLARE_DELEGATE_OneParam(FOnStoryRouteClicked, FName);

//스토리 줄기 하나에 대응하는 버튼 위젯, 목록 위젯이 동적으로 생성
UCLASS()
class PW_API UStoryRouteButton : public UUserWidget
{
	GENERATED_BODY()

public:
	//표시할 줄기와 클리어 여부 설정
	void InitRoute(const UStoryRouteData* route, bool bCleared);

	//클릭 통지, 목록 위젯이 바인딩
	FOnStoryRouteClicked OnClicked;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RouteButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;

	//클리어 표식, BP에서 체크 아이콘 등으로 구성
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> ClearedMark;

private:
	UFUNCTION()
	void HandleClicked();

	//이 버튼이 나타내는 줄기 식별자
	FName routeId;
};