//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TerrainComponent.generated.h"

class ATerrainBase;
class ACharacterBase;

//캐릭터가 체류 중인 지형을 관리하고 지형 훅을 디스패치
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PW_API UTerrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTerrainComponent();

protected:
	virtual void BeginPlay() override;

public:
	//지형 진입, ATerrainBase의 오버렙에서 호출
	void EnterTerrain(ATerrainBase* terrain);

	//지형 이탈, ATerrainBase의 오버렙에서 호출
	void ExitTerrain(ATerrainBase* terrain);

	const TArray<TObjectPtr<ATerrainBase>>& GetCurrentTerrains() const { return currentTerrains; }
	bool IsInTerrain() const { return currentTerrains.Num() > 0; }

	//턴 시작 시 체류 지형 훅 실행
	void DispatchTurnStart();

	//턴 종료 시 체류 지형 훅 실행
	void DispatchTurnEnd();

	//체류 지형들의 이동력 소모 배율 곱, 지형 밖이면 1.0
	float GetMovingPointCostMultiplier() const;

	//지형으로 얻는 전투 파생 스탯 보너스 누적, 재계산 덮어쓰기 방지 위해 컴포넌트에 저장
	void AddCombatStatBonus(float acc, float eva, float crit)
	{
		accuracyBonus += acc;
		evasionBonus += eva;
		criticalBonus += crit;
	}
	float GetAccuracyBonus() const { return accuracyBonus; }
	float GetEvasionBonus() const { return evasionBonus; }
	float GetCriticalBonus() const { return criticalBonus; }

private:
	UPROPERTY()
	TObjectPtr<ACharacterBase> ownerCharacter = nullptr;

	//전투 파생 스탯 보너스 누적치, 스탯 재계산 시 가산
	UPROPERTY(VisibleAnywhere)
	float accuracyBonus = 0.f;
	UPROPERTY(VisibleAnywhere)
	float evasionBonus = 0.f;
	UPROPERTY(VisibleAnywhere)
	float criticalBonus = 0.f;

	//현재 체류 중인 지형, 중첩 배치 허용
	UPROPERTY()
	TArray<TObjectPtr<ATerrainBase>> currentTerrains;
};