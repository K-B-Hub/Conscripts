// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TurnOrderEntryWidget.generated.h"

class UImage;
class ACharacterBase;

//턴 순서 항목의 진행 상태
UENUM()
enum class ETurnEntryState : uint8
{
	Past,
	Current,
	Upcoming
};

//턴 순서 칸 하나, 캐릭터 초상화와 진행 상태 표시
UCLASS()
class PW_API UTurnOrderEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//대응 캐릭터 설정
	void InitEntry(ACharacterBase* InCharacter);

	//진행 상태에 따라 시각 처리 갱신
	void SetState(ETurnEntryState InState);

protected:
	//BP에서 "PortraitImage" 이름으로 바인딩
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PortraitImage;

	//현재 턴 마커 (예: NOW 텍스트), BP에 있으면 자동 바인딩
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> NowMarker;

private:
	UPROPERTY()
	TObjectPtr<ACharacterBase> character = nullptr;
};
