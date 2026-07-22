//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Enum/UpgradeGrade.h"
#include "UpgradeTableData.generated.h"

class USkillBase;

//강화 후보 풀, 등급별로 스킬 클래스를 담음. 직업별 테이블은 파생 캐릭터 BP가, 공용 테이블은 GameInstance가 참조
UCLASS(BlueprintType)
class PW_API UUpgradeTableData : public UDataAsset
{
	GENERATED_BODY()

public:
	//하급 후보 풀
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	TArray<TSubclassOf<USkillBase>> lowPool;
	//중급 후보 풀
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	TArray<TSubclassOf<USkillBase>> midPool;
	//상급 후보 풀
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	TArray<TSubclassOf<USkillBase>> highPool;
	//최상급 후보 풀
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	TArray<TSubclassOf<USkillBase>> topPool;

	//등급별 후보 풀 조회
	const TArray<TSubclassOf<USkillBase>>& GetPool(EUpgradeGrade grade) const
	{
		switch (grade)
		{
		case EUpgradeGrade::Mid:  return midPool;
		case EUpgradeGrade::High: return highPool;
		case EUpgradeGrade::Top:  return topPool;
		default:                  return lowPool;
		}
	}
};
