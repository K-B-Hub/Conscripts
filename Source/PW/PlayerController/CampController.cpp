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
#include "GameInstance/PWGameInstance.h"
#include "Run/RunProgress.h"
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
		It->OnBeginCursorOver.AddDynamic(this, &ACampController::HandleCampHovered);
		It->OnEndCursorOver.AddDynamic(this, &ACampController::HandleCampUnhovered);
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

void ACampController::HandleCampHovered(AActor* touchedActor)
{
	if (ACampCenter* camp = Cast<ACampCenter>(touchedActor))
	{
		camp->SetHovered(true);
	}
}

void ACampController::HandleCampUnhovered(AActor* touchedActor)
{
	if (ACampCenter* camp = Cast<ACampCenter>(touchedActor))
	{
		camp->SetHovered(false);
	}
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

	//충원이 있으면 직업 선택이 이어지고, 그 흐름이 끝난 뒤에 떠난다
	if (option->recruitCount > 0)
	{
		BeginRecruitFlow(option->recruitCount);
		return;
	}

	camp->LeaveCamp();
}

void ACampController::BeginRecruitFlow(int32 count)
{
	ACampGameMode* camp = GetWorld() ? GetWorld()->GetAuthGameMode<ACampGameMode>() : nullptr;

	if (!recruitWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CampController] 충원 위젯이 없어 충원을 건너뜁니다"));
		if (camp) camp->LeaveCamp();
		return;
	}

	//부를 사람이 남지 않았으면 빈 화면을 띄우지 않고 그대로 출발한다
	const TArray<TSubclassOf<AAllyCharacterBase>> candidates = BuildRecruitCandidates();
	if (candidates.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[CampController] 부를 수 있는 동료가 없어 충원을 건너뜁니다"));
		if (camp) camp->LeaveCamp();
		return;
	}

	recruitWidgetInstance = CreateWidget<UCampRecruitWidget>(this, recruitWidgetClass);
	if (!recruitWidgetInstance)
	{
		if (camp) camp->LeaveCamp();
		return;
	}

	recruitWidgetInstance->OnRecruitConfirmed.BindUObject(this, &ACampController::RecruitCharacters);
	recruitWidgetInstance->AddToViewport();
	recruitWidgetInstance->Open(candidates, count);
}

void ACampController::RecruitCharacters(const TArray<TSubclassOf<AAllyCharacterBase>>& characterClasses)
{
	ACampGameMode* camp = GetWorld() ? GetWorld()->GetAuthGameMode<ACampGameMode>() : nullptr;
	if (!camp) return;

	for (const TSubclassOf<AAllyCharacterBase>& characterClass : characterClasses)
	{
		//후보 목록이 이미 걸렀지만 통지는 늦게 들어올 수 있어 여기서 한 번 더 본다
		if (!BuildRecruitCandidates().Contains(characterClass)) continue;

		//평균 레벨은 합류 전 기준이라 매번 다시 계산하지 않고 고정해도 되지만,
		//연속 충원 시 새 동료가 평균에 섞이지 않도록 여기서 그때그때 조회한다
		const int32 level = camp->GetAverageLevel();

		//ForceLevelUpTo가 쌓은 강화 대기 큐는 로스터에 실려 전투 첫 턴에 소비된다
		if (!camp->AddRecruit(characterClass, level))
		{
			UE_LOG(LogTemp, Warning, TEXT("[CampController] 빈 자리가 없어 충원하지 못했습니다"));
			break;
		}
	}

	if (recruitWidgetInstance)
	{
		recruitWidgetInstance->RemoveFromParent();
		recruitWidgetInstance = nullptr;
	}

	camp->LeaveCamp();
}

TArray<TSubclassOf<AAllyCharacterBase>> ACampController::BuildRecruitCandidates() const
{
	TArray<TSubclassOf<AAllyCharacterBase>> candidates;

	const UPWGameInstance* gameInstance = GetGameInstance<UPWGameInstance>();
	const ACampGameMode* camp = GetWorld() ? GetWorld()->GetAuthGameMode<ACampGameMode>() : nullptr;
	const URunProgress* runProgress = gameInstance ? gameInstance->GetRunProgress() : nullptr;
	if (!gameInstance || !camp || !runProgress) return candidates;

	//스토리면 줄기가 정한 인물들, 나머지 모드면 해금된 캐릭터가 후보 풀이다
	const TArray<TSubclassOf<AAllyCharacterBase>> pool = gameInstance->GetRunCharacterPool();
	const TArray<TObjectPtr<AAllyCharacterBase>>& campAllies = camp->GetCampAllies();

	for (const TSubclassOf<AAllyCharacterBase>& characterClass : pool)
	{
		if (!characterClass || runProgress->IsFallen(characterClass)) continue;

		//야영지에 서 있는 동료를 기준으로 본다, 로스터는 출발 전까지 갱신되지 않아 방금 합류한 동료를 놓친다
		bool bAlreadyHere = false;
		for (const TObjectPtr<AAllyCharacterBase>& ally : campAllies)
		{
			if (ally && ally->GetClass() == characterClass)
			{
				bAlreadyHere = true;
				break;
			}
		}

		if (!bAlreadyHere) candidates.Add(characterClass);
	}

	return candidates;
}