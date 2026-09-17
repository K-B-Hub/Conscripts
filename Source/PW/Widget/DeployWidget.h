//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeployWidget.generated.h"

class UButton;
class UPanelWidget;
class UTextBlock;
class UJobSelectButton;

//출격 배치 화면, 로스터 인원을 골라 구획에 놓는다
//실제 배치는 월드 클릭이 담당하므로 이 위젯은 "누구를 놓을지" 선택과 진행 상태 표시만 맡는다
UCLASS()
class PW_API UDeployWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//배치 상태가 바뀔 때마다 호출, 목록과 확정 버튼을 다시 그린다
	void RefreshList();

	//현재 선택된 로스터 인덱스, 선택이 없으면 INDEX_NONE
	int32 GetSelectedIndex() const { return selectedIndex; }

protected:
	virtual void NativeConstruct() override;

	//로스터 인원 목록
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> MemberContainer;

	//현재 해야 할 조작 안내
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HintText;

	//전원 배치되어야 활성화
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ConfirmButton;

	//목록 항목 위젯 클래스, BP에서 지정
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UJobSelectButton> memberButtonClass;

private:
	void HandleMemberClicked(int32 rosterIndex);

	UFUNCTION()
	void HandleConfirmClicked();

	//배치 대상으로 고른 로스터 인덱스
	int32 selectedIndex = INDEX_NONE;
};