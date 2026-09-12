//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Run/AllyRunState.h"
#include "FormationWidget.generated.h"

class UButton;
class UPanelWidget;
class UTextBlock;
class UJobSelectButton;

//캐릭터 편성 화면
//스토리는 줄기의 고정 편성을 읽기 전용으로 보여주고, 나머지 모드는 직업 목록에서 직접 고른다
UCLASS()
class PW_API UFormationWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	//현재 편성된 인원 목록
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> RosterContainer;

	//선택 가능한 직업 목록, 스토리에서는 숨긴다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> JobContainer;

	//직업 목록의 제목, 목록과 함께 숨겨야 하므로 따로 잡는다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> JobHeader;

	//"3 / 4" 형태의 편성 진행 표시
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CountText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ConfirmButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	//목록 항목 위젯 클래스, BP에서 지정
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UJobSelectButton> jobButtonClass;

private:
	//스토리면 고정 편성을 채우고 읽기 전용으로 전환
	void SetupForMode();

	void BuildJobList();
	void BuildRosterList();
	//편성 인원이 정원을 채웠을 때만 확정 가능
	void RefreshConfirmState();

	//직업 후보 클릭, 정원이 남아 있으면 편성에 추가
	void HandleJobClicked(int32 jobIndex);
	//편성된 인원 클릭, 목록에서 제외
	void HandleRosterClicked(int32 rosterIndex);

	UFUNCTION()
	void HandleConfirmClicked();
	UFUNCTION()
	void HandleBackClicked();

	//직업 클래스로 표시 문구 생성, 직업명이 비어 있으면 클래스명으로 대체
	static FText MakeJobLabel(TSubclassOf<AAllyCharacterBase> jobClass);

	//편성 중인 로스터
	//확정 전에는 RunProgress에 넣지 않는다 — StartRun이 객체를 새로 만들며 지워버리기 때문
	TArray<FAllyRunState> pendingRoster;

	//스토리 모드 여부, 편집을 막는다
	bool bReadOnly = false;

	//채워야 할 인원, 스토리는 고정 편성 인원 수와 같다
	int32 rosterCapacity = 0;
};