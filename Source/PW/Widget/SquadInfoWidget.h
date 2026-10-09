//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SquadInfoWidget.generated.h"

class UButton;
class UPanelWidget;
class USquadMemberRow;
class AAllyCharacterBase;
class ACharacterBase;

//닫기 요청 통지, 창을 띄운 컨트롤러가 바인딩
DECLARE_DELEGATE(FOnSquadInfoClosed);

//행 클릭 통지, 그 캐릭터의 상세 정보 창을 띄우는 쪽이 바인딩
DECLARE_DELEGATE_OneParam(FOnSquadMemberSelected, ACharacterBase*);

//부대 정보표, 아군 전원을 한 줄씩 나열한다
UCLASS()
class PW_API USquadInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//목록을 비우고 인원마다 행을 생성한다, 창이 열리는 시점에 한 번만 호출
	void InitSquad(const TArray<TObjectPtr<AAllyCharacterBase>>& members);

	FOnSquadInfoClosed OnClosed;
	FOnSquadMemberSelected OnMemberSelected;

protected:
	virtual void NativeConstruct() override;

	//목록에 채울 행 위젯, WBP에서 지정
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USquadMemberRow> rowClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

	//행 컨테이너, ScrollBox든 VerticalBox든 받는다
	//ScrollBox로 쓸 경우 부모 슬롯을 Fill로 둘 것 — Automatic이면 높이가 붕괴한다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> MemberList;

private:
	UFUNCTION()
	void HandleCloseClicked();

	//행에서 올라온 클릭을 그대로 위로 넘긴다
	void HandleRowClicked(ACharacterBase* target);
};
