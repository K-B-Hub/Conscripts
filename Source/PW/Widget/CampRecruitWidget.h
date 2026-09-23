//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CampRecruitWidget.generated.h"

class UPanelWidget;
class UTextBlock;
class UJobSelectButton;

//직업이 정해지면 해금 목록에서의 인덱스를 통지
DECLARE_DELEGATE_OneParam(FOnJobChosen, int32);

//충원 시 직업을 고르는 화면
//한 번 고를 때마다 한 명이 합류하므로 남은 인원만큼 반복해서 열린다
//야영지 충원과 전투 중 증원이 같은 목록을 쓰므로 컨트롤러를 알지 않고 델리게이트로 통지한다
UCLASS()
class PW_API UCampRecruitWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//남은 충원 인원 표시 갱신
	void SetRemaining(int32 remaining);

	//직업 선택 통지, 위젯을 띄운 컨트롤러가 바인딩
	FOnJobChosen OnJobChosen;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> JobContainer;

	//"남은 인원 2명" 같은 안내
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RemainingText;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UJobSelectButton> jobButtonClass;

private:
	void BuildJobList();

	void HandleJobClicked(int32 jobIndex);
};