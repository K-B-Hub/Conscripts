//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoryRouteSelectWidget.generated.h"

class UButton;
class UPanelWidget;
class UStoryRouteButton;

//스토리 줄기 선택 화면, GameInstance의 줄기 목록으로 버튼을 동적 생성
//줄기 개수가 미정이라 고정 슬롯 대신 컨테이너에 붙이는 방식을 쓴다
UCLASS()
class PW_API UStoryRouteSelectWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	//줄기 버튼이 붙을 컨테이너
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> RouteContainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	//줄기 항목 위젯 클래스, BP에서 지정
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UStoryRouteButton> routeButtonClass;

private:
	//줄기 목록으로 버튼을 만들어 컨테이너에 채움
	void BuildRouteList();

	void HandleRouteClicked(FName routeId);

	UFUNCTION()
	void HandleBackClicked();
};