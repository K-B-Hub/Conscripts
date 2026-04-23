// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "EnemyBase.generated.h"

class AAllyCharacterBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyDeath, AEnemyBase*, DeadEnemy, AAllyCharacterBase*, Killer);

UCLASS()
class PW_API AEnemyBase : public ACharacterBase
{
	GENERATED_BODY()

public:
	// 가장 최근 이 적을 공격한 아군 기록
	void SetLastAttacker(AAllyCharacterBase* Attacker);
	TWeakObjectPtr<AAllyCharacterBase> GetLastAttacker() const { return lastAttacker; }

	// 사망 시 경험치 분배용 델리게이트
	FOnEnemyDeath OnEnemyDeath;

	virtual void HandleDeath() override;

private:
	TWeakObjectPtr<AAllyCharacterBase> lastAttacker = nullptr;
};
