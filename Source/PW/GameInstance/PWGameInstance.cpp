// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstance/PWGameInstance.h"
#include "Run/RunProgress.h"
#include "Run/PWSaveGame.h"
#include "DataAsset/StoryRouteData.h"
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
	runProgress->StartRun(roster, storyRouteId);

	UE_LOG(LogTemp, Log, TEXT("[GameInstance] 런 시작, 모드=%d 줄기=%s 인원=%d"),
		static_cast<int32>(difficulty), *storyRouteId.ToString(), roster.Num());
}