#pragma once
#include "CoreMinimal.h"
#include "UpgradeGrade.generated.h"

UENUM(BlueprintType)
enum class EUpgradeGrade : uint8		//강화효과 등급, 출현 확률 60/30/7/3
{
	Low			UMETA(DisplayName = "Low"),		//하급 60%
	Mid			UMETA(DisplayName = "Mid"),		//중급 30%
	High		UMETA(DisplayName = "High"),	//상급 7%
	Top			UMETA(DisplayName = "Top")		//최상급 3%
};
