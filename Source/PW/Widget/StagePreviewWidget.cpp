//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/StagePreviewWidget.h"
#include "PlayerController/BattleController.h"
#include "GameInstance/PWGameInstance.h"
#include "Run/RunProgress.h"
#include "DataAsset/MissionData.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

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
		ProgressText->SetText(FText::FromString(FString::Printf(TEXT("남은 스테이지 %d"),
			FMath::Max(0, runProgress->GetTotalBattles() - runProgress->GetBattlesCleared()))));
	}

	ApplyStageIcon(NextIcon, next);
	if (NextNameText) NextNameText->SetText(DescribeStageName(next));
	if (NextDescText) NextDescText->SetText(DescribeStageDetail(next));

	//예고가 비면 지금 스테이지가 런의 마지막이다
	ApplyStageIcon(FollowingIcon, following);
	if (FollowingNameText)
	{
		FollowingNameText->SetText(following ? DescribeStageName(following) : FText::FromString(TEXT("런 종료")));
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
		CampCountText->SetText(FText::FromString(FString::Printf(TEXT("남은 야영 %d회"),
			runProgress->GetCampVisitsLeft())));
	}
}

void UStagePreviewWidget::ApplyStageIcon(UImage* image, const FStageEntry* stage) const
{
	if (!image) return;

	UTexture2D* icon = nullptr;
	if (stage)
	{
		icon = stage->Type == EStageType::Camp ? campIcon.Get()
			: stage->Mission ? stage->Mission->Icon.Get() : nullptr;
	}

	//크기는 위젯 배치가 정한다, 텍스처 해상도로 바꾸지 않는다
	if (icon) image->SetBrushFromTexture(icon, false);
	image->SetVisibility(icon ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}

FText UStagePreviewWidget::DescribeStageName(const FStageEntry* stage)
{
	if (!stage) return FText::GetEmpty();

	if (stage->Type == EStageType::Camp) return FText::FromString(TEXT("야영지"));

	//임무를 지정하지 않은 전투도 있을 수 있어 종류 이름으로 물러선다
	if (stage->Mission && !stage->Mission->TypeName.IsEmpty()) return stage->Mission->TypeName;

	return FText::FromString(TEXT("전투"));
}

FText UStagePreviewWidget::DescribeStageDetail(const FStageEntry* stage)
{
	if (!stage) return FText::GetEmpty();

	if (stage->Type == EStageType::Camp) return FText::FromString(TEXT("부대를 정비하고 동료를 충원한다"));

	return stage->Mission ? stage->Mission->Description : FText::GetEmpty();
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