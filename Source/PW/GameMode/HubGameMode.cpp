//Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/HubGameMode.h"
#include "PlayerController/HubController.h"
#include "GameInstance/BgmSubsystem.h"

AHubGameMode::AHubGameMode()
{
	PlayerControllerClass = AHubController::StaticClass();

	//UI만 표시하므로 조작 대상 폰이 필요 없음
	DefaultPawnClass = nullptr;
}

void AHubGameMode::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetSubsystem<UBgmSubsystem>()->Play(menuBgm);
}