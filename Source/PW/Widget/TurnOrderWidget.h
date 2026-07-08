// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TurnOrderWidget.generated.h"

class UScrollBox;
class UTurnOrderEntryWidget;
class ACharacterBase;
class ABattleGameMode;

//턴 순서 전체 표시 위젯, GameMode의 턴 순서 변경을 구독해 갱신
UCLASS()
class PW_API UTurnOrderWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	//BP에서 "TurnScrollBox" 이름으로 바인딩
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> TurnScrollBox;

	//개별 턴 칸으로 사용할 위젯 BP 클래스
	UPROPERTY(EditDefaultsOnly, Category = "TurnOrder")
	TSubclassOf<UTurnOrderEntryWidget> EntryClass;

private:
	//턴 순서 배열 변경 시(라운드 시작, 사망) 칸 전체 재생성
	void RebuildEntries(const TArray<ACharacterBase*>& InTurnOrder, int32 InCurrentIndex);

	//턴 전환 시 각 칸의 상태만 갱신하고 현재 턴으로 스크롤
	void SetCurrentTurn(int32 InCurrentIndex);

	UPROPERTY()
	TArray<TObjectPtr<UTurnOrderEntryWidget>> entries;

	ABattleGameMode* cachedGameMode = nullptr;
};
