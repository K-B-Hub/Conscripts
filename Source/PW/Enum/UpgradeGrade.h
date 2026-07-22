#pragma once
#include "CoreMinimal.h"
#include "UpgradeGrade.generated.h"

UENUM(BlueprintType)
enum class EUpgradeGrade : uint8		//강화효과 등급, 출현 확률 55/30/10/5
{
	Low			UMETA(DisplayName = "Low"),		//하급 55%
	Mid			UMETA(DisplayName = "Mid"),		//중급 30%
	High		UMETA(DisplayName = "High"),	//상급 10%
	Top			UMETA(DisplayName = "Top")		//최상급 5%
};
