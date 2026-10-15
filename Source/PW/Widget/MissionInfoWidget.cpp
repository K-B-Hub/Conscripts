//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/MissionInfoWidget.h"
#include "GameMode/BattleGameMode.h"
#include "DataAsset/MissionData.h"
#include "Components/TextBlock.h"

void UMissionInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(false);

	const ABattleGameMode* gameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ABattleGameMode>() : nullptr;
	const UMissionData* mission = gameMode ? gameMode->GetCurrentMission() : nullptr;

	//런 없이 전투 맵을 직접 연 경우 임무가 없다, 예고 화면과 같은 폴백으로 종류 이름만 남긴다
	if (MissionTypeText)
	{
		const bool bHasType = mission && !mission->TypeName.IsEmpty();
		MissionTypeText->SetText(bHasType ? mission->TypeName : FText::FromString(TEXT("전투")));
	}

	//설명이 없으면 빈 줄이 자리만 차지하므로 접는다
	if (MissionDescriptionText)
	{
		const bool bHasDescription = mission && !mission->Description.IsEmpty();
		MissionDescriptionText->SetText(bHasDescription ? mission->Description : FText::GetEmpty());
		MissionDescriptionText->SetVisibility(bHasDescription
			? ESlateVisibility::HitTestInvisible
			: ESlateVisibility::Collapsed);
	}
}