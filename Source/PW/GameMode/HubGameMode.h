//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HubGameMode.generated.h"

//메인메뉴·모드선택·설정·줄기선택·편성을 담는 허브 레벨의 게임모드
//UI 전용이라 캐릭터도 조작 대상 폰도 없고, 화면 전환은 HubController가 담당
UCLASS()
class PW_API AHubGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHubGameMode();
};