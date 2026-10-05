//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeployWidget.generated.h"

class UButton;
class UImage;
class UPanelWidget;
class UTextBlock;
class UTexture2D;
class UDeployMemberEntry;
class AAllyCharacterBase;
struct FAllyRunState;

//출격 배치 화면, 로스터 인원을 골라 구획에 놓는다
//실제 배치는 월드 클릭이 담당하므로 이 위젯은 "누구를 놓을지" 선택과 진행 상태 표시만 맡는다
UCLASS()
class PW_API UDeployWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//배치 상태가 바뀔 때마다 호출, 목록·정보 패널·버튼 상태를 다시 그린다
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

	//목록 머리의 "배치 수 / 전체 수"
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DeployCountText;

	//전원 배치되어야 활성화
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ConfirmButton;

	//배치된 인원을 모두 거둔다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> ResetButton;

	//전원 배치 시에만 표시, 표시/숨김만 토글하고 문구는 BP 정적 텍스트를 그대로 쓴다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> DeployCompletePanel;

	//선택된 인원의 정보 패널, 선택이 없으면 숨긴다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> InfoPanel;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> InfoJobImage;
	//개체 이름
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> InfoNameText;
	//직업 이름
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> InfoJobText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> InfoLevelText;
	//최대 체력
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> InfoHpText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> InfoAtkText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> InfoDefText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> InfoSpeedText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> InfoSkillText;
	//최대 행동력
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> InfoApText;

	//목록 항목 위젯 클래스, BP에서 지정
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDeployMemberEntry> memberEntryClass;

private:
	//로스터 수가 바뀌었을 때만 항목을 새로 만든다, 매번 다시 만들면 스크롤 위치가 초기화된다
	void RebuildEntries(const TArray<FAllyRunState>& roster);

	//선택된 인원으로 정보 패널 갱신, 유효하지 않으면 숨긴다
	void RefreshInfoPanel(const TArray<FAllyRunState>& roster);

	//직업 이름, 비어 있으면 클래스 이름으로 대체
	static FText GetJobName(TSubclassOf<AAllyCharacterBase> jobClass);
	static UTexture2D* GetJobIcon(TSubclassOf<AAllyCharacterBase> jobClass);

	void HandleMemberClicked(int32 rosterIndex);

	UFUNCTION()
	void HandleConfirmClicked();

	UFUNCTION()
	void HandleResetClicked();

	//배치 대상으로 고른 로스터 인덱스
	int32 selectedIndex = INDEX_NONE;

	//로스터 인덱스 순서 그대로의 항목 위젯
	UPROPERTY()
	TArray<TObjectPtr<UDeployMemberEntry>> memberEntries;
};
