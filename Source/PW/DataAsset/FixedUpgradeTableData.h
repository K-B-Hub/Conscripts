//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FixedUpgradeTableData.generated.h"

class USkillBase;

//직업별 레벨 고정 강화 매핑 전용, 파생 직업 BP가 참조. 확률 후보 풀과 성격이 달라 분리
UCLASS(BlueprintType)
class PW_API UFixedUpgradeTableData : public UDataAsset
{
	GENERATED_BODY()

public:
	//레벨→고정 강화 스킬. 배열 순서가 아닌 레벨을 명시적 키로 두어 순서 의존을 제거
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	TMap<int32, TSubclassOf<USkillBase>> fixedUpgrades;

	//해당 레벨의 고정 강화 스킬, 없으면 nullptr
	TSubclassOf<USkillBase> GetFixedUpgrade(int32 level) const
	{
		const TSubclassOf<USkillBase>* found = fixedUpgrades.Find(level);
		return found ? *found : nullptr;
	}
};