// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstance/PWGameInstance.h"
#include "Run/RunProgress.h"
#include "Run/PWSaveGame.h"
#include "DataAsset/StoryRouteData.h"
#include "DataAsset/RunModeData.h"
#include "Characters/AllyCharacterBase.h"
#include "Kismet/GameplayStatics.h"

namespace
{
	//세이브 슬롯은 하나뿐이라 밖으로 노출하지 않음
	const TCHAR* SaveSlotName = TEXT("PWProgress");
	constexpr int32 SaveUserIndex = 0;
}

void UPWGameInstance::Init()
{
	Super::Init();

	runProgress = NewObject<URunProgress>(this);

	const bool bSlotExists = UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex);
	if (bSlotExists)
	{
		saveGame = Cast<UPWSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));
	}

	//슬롯 유무와 로드 성공은 별개다. 클래스가 바뀌어 로드가 실패해도 슬롯은 존재하므로 따로 기록
	const bool bLoaded = saveGame != nullptr;

	//세이브가 없거나 로드에 실패해도 이후 코드가 널 검사 없이 같은 경로를 타도록 보장
	if (!saveGame)
	{
		saveGame = Cast<UPWSaveGame>(UGameplayStatics::CreateSaveGameObject(UPWSaveGame::StaticClass()));
	}

	UE_LOG(LogTemp, Log, TEXT("[GameInstance] 세이브 슬롯 %s / 로드 %s / 클리어한 줄기 %d개"),
		bSlotExists ? TEXT("있음") : TEXT("없음"),
		bLoaded ? TEXT("성공") : TEXT("실패, 새로 생성"),
		saveGame ? saveGame->clearedStoryRoutes.Num() : 0);
}

void UPWGameInstance::SaveProgress()
{
	if (!saveGame) return;

	//반환값을 버리면 디스크 쓰기 실패가 조용히 묻힌다
	const bool bSaved = UGameplayStatics::SaveGameToSlot(saveGame, SaveSlotName, SaveUserIndex);

	UE_LOG(LogTemp, Log, TEXT("[GameInstance] 진행도 저장 %s / 클리어한 줄기 %d개"),
		bSaved ? TEXT("성공") : TEXT("실패"),
		saveGame->clearedStoryRoutes.Num());
}

bool UPWGameInstance::TravelToCurrentStage(const UObject* worldContext)
{
	const FStageEntry* stage = runProgress ? runProgress->GetCurrentStage() : nullptr;
	if (!stage || stage->Map.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameInstance] 진행할 스테이지가 없습니다"));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("[GameInstance] 스테이지 %d로 이동"), runProgress->GetStageIndex() + 1);
	UGameplayStatics::OpenLevelBySoftObjectPtr(worldContext, stage->Map);
	return true;
}

void UPWGameInstance::EndRunAndReturnToHub(const UObject* worldContext, bool bCleared)
{
	//스토리가 아니면 routeId가 비어 있어 기록이 그냥 지나간다
	const FName routeId = runProgress ? runProgress->GetStoryRouteId() : NAME_None;
	if (bCleared && !routeId.IsNone() && saveGame)
	{
		saveGame->RegisterStoryClear(routeId);

		//다음 실행에서 해금이 살아 있어야 하므로 즉시 디스크에 쓴다
		SaveProgress();
	}

	if (runProgress) runProgress->EndRun();

	if (hubMap.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameInstance] 허브 맵이 지정되지 않아 복귀할 수 없습니다"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[GameInstance] 런 종료(%s), 허브로 복귀"),
		bCleared ? TEXT("완주") : TEXT("중단"));
	UGameplayStatics::OpenLevelBySoftObjectPtr(worldContext, hubMap);
}

TArray<TSubclassOf<AAllyCharacterBase>> UPWGameInstance::GetUnlockedJobs() const
{
	TArray<TSubclassOf<AAllyCharacterBase>> jobs = selectableJobs;

	//세이브에는 줄기만 남으므로 해금 직업은 매번 매핑에서 다시 구한다
	for (const UStoryRouteData* route : storyRoutes)
	{
		if (!route || !saveGame || !saveGame->IsStoryRouteCleared(route->routeId)) continue;

		//여러 줄기가 같은 직업을 열 수 있다
		for (const TSubclassOf<AAllyCharacterBase>& job : route->unlockedJobs)
		{
			if (job) jobs.AddUnique(job);
		}
	}

	return jobs;
}

const UStoryRouteData* UPWGameInstance::FindStoryRoute(FName routeId) const
{
	if (routeId.IsNone()) return nullptr;

	for (const UStoryRouteData* route : storyRoutes)
	{
		if (route && route->routeId == routeId) return route;
	}
	return nullptr;
}

void UPWGameInstance::StartRun(const TArray<FAllyRunState>& roster, FName storyRouteId)
{
	//이전 런의 값이 새 런에 새어 들지 않도록 필드 리셋이 아니라 객체를 통째로 교체
	runProgress = NewObject<URunProgress>(this);

	//스토리는 줄기가 시퀀스를 통째로 갖고, 나머지는 규칙으로 만들어 간다
	if (difficulty == EGameDifficulty::Stage)
	{
		const UStoryRouteData* route = FindStoryRoute(storyRouteId);
		const TArray<FStageEntry>& stages =
			(route && route->stages.Num() > 0) ? route->stages : defaultStageSequence;

		runProgress->StartFixedRun(roster, storyRouteId, stages);
	}
	else
	{
		URunModeData* mode = (difficulty == EGameDifficulty::Nightmare) ? nightmareMode : roguelikeMode;
		if (mode)
		{
			runProgress->StartGeneratedRun(roster, mode);
		}
		else
		{
			//모드 설정을 아직 만들지 않았어도 흐름은 돌아가야 한다
			UE_LOG(LogTemp, Warning, TEXT("[GameInstance] 모드 설정이 없어 기본 시퀀스로 진행합니다"));
			runProgress->StartFixedRun(roster, NAME_None, defaultStageSequence);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[GameInstance] 런 시작, 모드=%d 줄기=%s 인원=%d 목표 전투=%d회"),
		static_cast<int32>(difficulty), *storyRouteId.ToString(), roster.Num(), runProgress->GetTotalBattles());
}