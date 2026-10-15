//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MissionInfoWidget.generated.h"

class UTextBlock;

//전투 HUD의 임무 표시, 지속 HUD의 자식으로 놓인다
//임무는 전투 중 바뀌지 않으므로 생성 시점에 한 번만 채운다
UCLASS()
class PW_API UMissionInfoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MissionTypeText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MissionDescriptionText;
};