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
//스토리는 줄기의 고정 편성을 읽기 전용으로 보여주고, 나머지 모드는 캐릭터 목록에서 직접 고른다
//같은 캐릭터를 두 명 둘 수 없으므로 편성한 캐릭터는 후보 목록에서 사라진다
UCLASS()
class PW_API UFormationWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	//현재 편성된 인원 목록
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> RosterContainer;

	//선택 가능한 캐릭터 목록, 이미 편성한 캐릭터는 여기서 사라진다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> JobContainer;

	//캐릭터 목록의 제목
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
	//후보 풀과 정원을 모드에 맞춰 정한다
	void SetupForMode();

	void BuildJobList();
	void BuildRosterList();
	//편성 인원이 정원을 채웠을 때만 확정 가능
	void RefreshConfirmState();

	//캐릭터 후보 클릭, 정원이 남아 있으면 편성에 추가
	void HandleCharacterClicked(TSubclassOf<AAllyCharacterBase> characterClass);
	//편성된 인원 클릭, 목록에서 제외
	void HandleRosterClicked(TSubclassOf<AAllyCharacterBase> characterClass);

	UFUNCTION()
	void HandleConfirmClicked();
	UFUNCTION()
	void HandleBackClicked();

	//편성 중인 로스터에서의 위치, 없으면 INDEX_NONE
	//클래스가 곧 한 명이라 로스터의 고유 키로 쓸 수 있다
	int32 IndexOfPending(TSubclassOf<AAllyCharacterBase> characterClass) const;

	//편성 중인 로스터
	//확정 전에는 RunProgress에 넣지 않는다 — StartRun이 객체를 새로 만들며 지워버리기 때문
	TArray<FAllyRunState> pendingRoster;

	//이번 런의 캐릭터 후보 풀, 스토리는 줄기 목록이고 나머지는 해금 목록이다
	TArray<TSubclassOf<AAllyCharacterBase>> characterPool;

	//스토리 줄기 식별자, 스토리에서 들어왔는지와 뒤로 갈 화면을 함께 가리킨다
	FName storyRouteId = NAME_None;

	//채워야 할 인원, 후보가 정원보다 적으면 후보 수로 줄인다
	int32 rosterCapacity = 0;
};