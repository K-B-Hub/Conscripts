#pragma once
#include "CoreMinimal.h"
#include "GameDifficulty.generated.h"

UENUM(BlueprintType)
enum class EGameDifficulty : uint8		//게임 난이도, 경험치 계수 테이블의 열 인덱스로도 사용
{
	Stage       UMETA(DisplayName = "Stage"),		//스테이지 모드
	Roguelike   UMETA(DisplayName = "Roguelike"),	//로그라이크 모드
	Nightmare   UMETA(DisplayName = "Nightmare")	//악몽 모드
};
