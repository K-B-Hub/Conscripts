//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoryRouteButton.generated.h"

class UImageButton;
class UStoryRouteData;

//줄기 항목을 클릭하면 대응 줄기 ID를 통지
DECLARE_DELEGATE_OneParam(FOnStoryRouteClicked, FName);

//스토리 줄기 하나에 대응하는 버튼 위젯, 목록 위젯이 동적으로 생성
UCLASS()
class PW_API UStoryRouteButton : public UUserWidget
{
	GENERATED_BODY()

public:
	//표시할 줄기 설정
	void InitRoute(const UStoryRouteData* route);

	//클릭 통지, 목록 위젯이 바인딩
	FOnStoryRouteClicked OnClicked;

protected:
	virtual void NativeConstruct() override;

	//줄기 이름은 버튼 아트에 들어 있어 별도 라벨을 두지 않는다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImageButton> RouteButton;

	//BP에 남은 클리어 표식, 띄우지 않으므로 접기 위해서만 바인딩한다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> ClearedMark;

private:
	UFUNCTION()
	void HandleClicked();

	//이 버튼이 나타내는 줄기 식별자
	FName routeId;
};