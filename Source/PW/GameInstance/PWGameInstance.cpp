// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstance/PWGameInstance.h"
#include "Run/RunProgress.h"
#include "Run/PWSaveGame.h"
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

	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex))
	{
		saveGame = Cast<UPWSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));
	}

	//세이브가 없거나 로드에 실패해도 이후 코드가 널 검사 없이 같은 경로를 타도록 보장
	if (!saveGame)
	{
		saveGame = Cast<UPWSaveGame>(UGameplayStatics::CreateSaveGameObject(UPWSaveGame::StaticClass()));
	}

	UE_LOG(LogTemp, Log, TEXT("[GameInstance] 진행도 로드 완료, 클리어한 스토리 줄기 %d개"),
		saveGame ? saveGame->clearedStoryRoutes.Num() : 0);
}

void UPWGameInstance::SaveProgress()
{
	if (!saveGame) return;

	UGameplayStatics::SaveGameToSlot(saveGame, SaveSlotName, SaveUserIndex);
}

void UPWGameInstance::StartRun(FName storyRouteId)
{
	//이전 런의 값이 새 런에 새어 들지 않도록 필드 리셋이 아니라 객체를 통째로 교체
	runProgress = NewObject<URunProgress>(this);
	runProgress->StartRun(storyRouteId);
}