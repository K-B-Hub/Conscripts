// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Enum/GameDifficulty.h"
#include "PWGameInstance.generated.h"

class UStressPoolData;
class UUpgradeTableData;

//레벨 전환 간 유지되는 런 상태 보관(난이도, 추후 캐릭터 상태·특전 등)
UCLASS()
class PW_API UPWGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Game")
	EGameDifficulty GetDifficulty() const { return difficulty; }

	//메인메뉴에서 전투 시작 시 호출
	UFUNCTION(BlueprintCallable, Category = "Game")
	void SetDifficulty(EGameDifficulty newDifficulty) { difficulty = newDifficulty; }

	//스트레스 이벤트 풀 조회, 미설정 시 nullptr
	UStressPoolData* GetStressPool() const { return stressPool; }

	//공용 강화 후보 풀 조회, 미설정 시 nullptr
	UUpgradeTableData* GetCommonUpgradeTable() const { return commonUpgradeTable; }

protected:
	//현재 런의 난이도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	EGameDifficulty difficulty = EGameDifficulty::Stage;

	//스트레스 한계 도달 시 사용할 이벤트 풀, 런 전체에서 공유
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stress")
	TObjectPtr<UStressPoolData> stressPool;

	//모든 직업이 공통으로 얻을 수 있는 강화 후보 풀, 런 전체에서 공유
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	TObjectPtr<UUpgradeTableData> commonUpgradeTable;
};
