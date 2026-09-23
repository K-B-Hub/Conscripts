//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/StagePreviewWidget.h"
#include "PlayerController/BattleController.h"
#include "GameInstance/PWGameInstance.h"
#include "Run/RunProgress.h"
#include "DataAsset/MissionData.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UStagePreviewWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (NextButton) NextButton->OnClicked.AddDynamic(this, &UStagePreviewWidget::HandleNextClicked);
	if (CampButton) CampButton->OnClicked.AddDynamic(this, &UStagePreviewWidget::HandleCampClicked);

	Refresh();
}

void UStagePreviewWidget::Refresh()
{
	const UWorld* world = GetWorld();
	const UPWGameInstance* gameInstance = world ? world->GetGameInstance<UPWGameInstance>() : nullptr;
	const URunProgress* runProgress = gameInstance ? gameInstance->GetRunProgress() : nullptr;
	if (!runProgress) return;

	const FStageEntry* next = runProgress->GetUpcomingStage(0);
	const FStageEntry* following = runProgress->GetUpcomingStage(1);
	const bool bCamp = next && next->Type == EStageType::Camp;

	if (ProgressText)
	{
		ProgressText->SetText(FText::FromString(FString::Printf(TEXT("전투 %d / %d 완료"),
			runProgress->GetBattlesCleared(), runProgress->GetTotalBattles())));
	}

	if (StageText)
	{
		StageText->SetText(FText::FromString(FString::Printf(TEXT("다음: %s"),
			*DescribeStage(next).ToString())));
	}

	if (FollowingStageText)
	{
		//예고가 비면 지금 스테이지가 런의 마지막이다
		const FString label = following
			? FString::Printf(TEXT("그 다음: %s"), *DescribeStage(following).ToString())
			: FString(TEXT("그 다음: 런 종료"));

		FollowingStageText->SetText(FText::FromString(label));
	}

	//야영지가 이미 다음이면 더 끼워 넣어도 의미가 없다
	const bool bCanVisit = runProgress->GetCampVisitsLeft() > 0 && !bCamp;

	if (CampButton)
	{
		CampButton->SetVisibility(bCanVisit ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (CampCountText)
	{
		CampCountText->SetVisibility(bCanVisit ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		CampCountText->SetText(FText::FromString(FString::Printf(TEXT("야영지 들르기 (남은 %d회)"),
			runProgress->GetCampVisitsLeft())));
	}
}

FText UStagePreviewWidget::DescribeStage(const FStageEntry* stage)
{
	if (!stage) return FText::GetEmpty();

	if (stage->Type == EStageType::Camp) return FText::FromString(TEXT("야영지"));

	//임무를 지정하지 않은 전투도 있을 수 있어 종류 이름으로 물러선다
	if (stage->Mission && !stage->Mission->Description.IsEmpty()) return stage->Mission->Description;

	return FText::FromString(TEXT("전투"));
}

void UStagePreviewWidget::HandleNextClicked()
{
	if (ABattleController* battleController = GetOwningPlayer<ABattleController>())
	{
		battleController->TravelToNextStage();
	}
}

void UStagePreviewWidget::HandleCampClicked()
{
	ABattleController* battleController = GetOwningPlayer<ABattleController>();
	if (!battleController) return;

	//야영지가 다음 자리를 차지했으므로 예고 내용을 다시 채운다
	if (battleController->InsertCampVisit()) Refresh();
}