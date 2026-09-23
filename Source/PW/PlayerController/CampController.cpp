//Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerController/CampController.h"
#include "Actors/CampSpawnPoint.h"
#include "Actors/CampCenter.h"
#include "Characters/AllyCharacterBase.h"
#include "DataAsset/CampDialogueData.h"
#include "DataAsset/CampOptionData.h"
#include "GameMode/CampGameMode.h"
#include "Widget/CampOptionWidget.h"
#include "Widget/CampRecruitWidget.h"
#include "Widget/UpgradeSelectWidget.h"
#include "GameInstance/PWGameInstance.h"
#include "DataAsset/UpgradeLibrary.h"
#include "EngineUtils.h"

ACampController::ACampController()
{
	//말풍선과 캠프 클릭이 모두 커서 이벤트에 의존한다
	bShowMouseCursor = true;
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;
}

void ACampController::BeginPlay()
{
	Super::BeginPlay();

	//허브의 FInputModeUIOnly가 뷰포트에 남긴 SetIgnoreInput(true)를 되돌린다
	//야영지는 월드 클릭과 UMG 버튼을 모두 쓰므로 GameAndUI
	SetInputMode(FInputModeGameAndUI());

	BindCampActors();
}

void ACampController::BindCampActors()
{
	//자리와 캠프는 레벨에 미리 배치된 액터라 GameMode의 스폰을 기다릴 필요가 없다
	//자리에 누가 섰는지는 hover 시점에 조회하므로 지금 비어 있어도 무방하다
	for (TActorIterator<ACampSpawnPoint> It(GetWorld()); It; ++It)
	{
		It->OnBeginCursorOver.AddDynamic(this, &ACampController::HandlePointHovered);
		It->OnEndCursorOver.AddDynamic(this, &ACampController::HandlePointUnhovered);
	}

	int32 campCount = 0;
	for (TActorIterator<ACampCenter> It(GetWorld()); It; ++It)
	{
		It->OnClicked.AddDynamic(this, &ACampController::HandleCampClicked);
		++campCount;
	}

	if (campCount == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CampController] 레벨에 ACampCenter가 없어 정비를 열 수 없습니다"));
	}
}

void ACampController::HandlePointHovered(AActor* touchedActor)
{
	const ACampSpawnPoint* point = Cast<ACampSpawnPoint>(touchedActor);
	AAllyCharacterBase* ally = point ? point->GetOccupant() : nullptr;

	if (!ally) return;

	ally->ShowCampLine(dialogueData ? dialogueData->PickLine(ally) : FText::GetEmpty());
}

void ACampController::HandlePointUnhovered(AActor* touchedActor)
{
	const ACampSpawnPoint* point = Cast<ACampSpawnPoint>(touchedActor);
	AAllyCharacterBase* ally = point ? point->GetOccupant() : nullptr;
	if (!ally) return;

	ally->HideCampLine();
}

void ACampController::HandleCampClicked(AActor* touchedActor, FKey buttonPressed)
{
	//이미 열려 있으면 다시 만들지 않는다
	if (optionWidgetInstance || !optionWidgetClass) return;

	optionWidgetInstance = CreateWidget<UCampOptionWidget>(this, optionWidgetClass);
	if (!optionWidgetInstance) return;

	optionWidgetInstance->AddToViewport();
}

void ACampController::CloseCampOptions()
{
	if (!optionWidgetInstance) return;

	optionWidgetInstance->RemoveFromParent();
	optionWidgetInstance = nullptr;
}

void ACampController::ChooseCampOption(int32 optionIndex)
{
	ACampGameMode* camp = GetWorld() ? GetWorld()->GetAuthGameMode<ACampGameMode>() : nullptr;
	if (!camp) return;

	const TArray<TObjectPtr<UCampOptionData>>& options = camp->GetCampOptions();
	if (!options.IsValidIndex(optionIndex) || !options[optionIndex]) return;

	const UCampOptionData* option = options[optionIndex];
	CloseCampOptions();

	option->ApplyRestoration(camp);

	//충원이 있으면 직업·강화 선택이 이어지고, 그 흐름이 끝난 뒤에 떠난다
	if (option->recruitCount > 0)
	{
		BeginRecruitFlow(option->recruitCount);
		return;
	}

	camp->LeaveCamp();
}

void ACampController::BeginRecruitFlow(int32 count)
{
	remainingRecruits = count;

	if (!recruitWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CampController] 충원 위젯이 없어 충원을 건너뜁니다"));
		AdvanceUpgradeFlow();
		return;
	}

	recruitWidgetInstance = CreateWidget<UCampRecruitWidget>(this, recruitWidgetClass);
	if (!recruitWidgetInstance)
	{
		AdvanceUpgradeFlow();
		return;
	}

	recruitWidgetInstance->OnJobChosen.BindUObject(this, &ACampController::RecruitJob);
	recruitWidgetInstance->AddToViewport();
	recruitWidgetInstance->SetRemaining(remainingRecruits);
}

void ACampController::RecruitJob(int32 jobIndex)
{
	ACampGameMode* camp = GetWorld() ? GetWorld()->GetAuthGameMode<ACampGameMode>() : nullptr;
	const UPWGameInstance* gameInstance = GetGameInstance<UPWGameInstance>();
	if (!camp || !gameInstance || remainingRecruits <= 0) return;

	const TArray<TSubclassOf<AAllyCharacterBase>> jobs = gameInstance->GetUnlockedJobs();
	if (!jobs.IsValidIndex(jobIndex) || !jobs[jobIndex]) return;

	//평균 레벨은 합류 전 기준이라 매번 다시 계산하지 않고 고정해도 되지만,
	//연속 충원 시 새 동료가 평균에 섞이지 않도록 여기서 그때그때 조회한다
	const int32 level = camp->GetAverageLevel();

	const FText& jobName = jobs[jobIndex]->GetDefaultObject<AAllyCharacterBase>()->jobName;
	const FString name = FString::Printf(TEXT("%s %d"),
		*(jobName.IsEmpty() ? FText::FromString(jobs[jobIndex]->GetName()) : jobName).ToString(),
		camp->GetCampAllies().Num() + 1);

	AAllyCharacterBase* recruit = camp->AddRecruit(jobs[jobIndex], name, level);
	if (!recruit)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CampController] 빈 자리가 없어 충원하지 못했습니다"));
		remainingRecruits = 0;
	}
	else
	{
		//ForceLevelUpTo가 쌓아둔 강화 대기 큐를 뒤에서 소비한다
		upgradePendingAllies.Add(recruit);
		--remainingRecruits;
	}

	if (remainingRecruits > 0)
	{
		if (recruitWidgetInstance) recruitWidgetInstance->SetRemaining(remainingRecruits);
		return;
	}

	if (recruitWidgetInstance)
	{
		recruitWidgetInstance->RemoveFromParent();
		recruitWidgetInstance = nullptr;
	}

	AdvanceUpgradeFlow();
}

void ACampController::AdvanceUpgradeFlow()
{
	//현재 동료의 대기 강화가 남아 있으면 계속 고르게 한다
	if (IsValid(currentUpgradeAlly) && currentUpgradeAlly->GetPendingUpgradeCount() > 0)
	{
		ShowUpgradeSelect();
		return;
	}

	//다음 동료로 넘어간다
	while (upgradePendingAllies.Num() > 0)
	{
		currentUpgradeAlly = upgradePendingAllies[0];
		upgradePendingAllies.RemoveAt(0);

		if (IsValid(currentUpgradeAlly) && currentUpgradeAlly->GetPendingUpgradeCount() > 0)
		{
			ShowUpgradeSelect();
			return;
		}
	}

	//모든 충원 동료의 강화 선택이 끝났다
	currentUpgradeAlly = nullptr;

	if (ACampGameMode* camp = GetWorld() ? GetWorld()->GetAuthGameMode<ACampGameMode>() : nullptr)
	{
		camp->LeaveCamp();
	}
}

void ACampController::ShowUpgradeSelect()
{
	if (!IsValid(currentUpgradeAlly) || !upgradeSelectWidgetClass)
	{
		//위젯이 없으면 선택 없이 큐만 비우고 진행한다, 야영지에 갇히지 않도록
		if (IsValid(currentUpgradeAlly)) currentUpgradeAlly->ConsumePendingUpgrade();
		AdvanceUpgradeFlow();
		return;
	}

	const UPWGameInstance* gameInstance = GetGameInstance<UPWGameInstance>();
	UUpgradeTableData* commonTable = gameInstance ? gameInstance->GetCommonUpgradeTable() : nullptr;

	//후보 생성은 전투와 같은 경로를 쓴다
	const TArray<TSubclassOf<USkillBase>> choices =
		UUpgradeLibrary::BuildPendingChoices(currentUpgradeAlly, commonTable);

	//후보가 없으면 큐만 소비하고 다음으로
	if (choices.Num() == 0)
	{
		currentUpgradeAlly->ConsumePendingUpgrade();
		AdvanceUpgradeFlow();
		return;
	}

	upgradeSelectWidgetInstance = CreateWidget<UUpgradeSelectWidget>(this, upgradeSelectWidgetClass);
	if (!upgradeSelectWidgetInstance)
	{
		currentUpgradeAlly->ConsumePendingUpgrade();
		AdvanceUpgradeFlow();
		return;
	}

	upgradeSelectWidgetInstance->SetChoices(choices);
	upgradeSelectWidgetInstance->OnUpgradeChosen.BindUObject(this, &ACampController::OnUpgradeChosen);
	upgradeSelectWidgetInstance->AddToViewport(10);
}

void ACampController::OnUpgradeChosen(TSubclassOf<USkillBase> chosen)
{
	if (IsValid(currentUpgradeAlly))
	{
		currentUpgradeAlly->AcquireUpgrade(chosen);
		currentUpgradeAlly->ConsumePendingUpgrade();
	}

	if (IsValid(upgradeSelectWidgetInstance))
	{
		upgradeSelectWidgetInstance->RemoveFromParent();
		upgradeSelectWidgetInstance = nullptr;
	}

	AdvanceUpgradeFlow();
}