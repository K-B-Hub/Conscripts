// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"

class UBuffBase;
class ACharacterBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PW_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBuffComponent();

protected:
	virtual void BeginPlay() override;

public:
	//새 버프 적용
	void AddBuff(TSubclassOf<UBuffBase> buffClass, ACharacterBase* caster);

	//인덱스로 제거
	void RemoveBuffAt(int32 Index);

	//턴 시작시 적용
	void OnTurnStart();

	//턴 종료시 적용 및 만료 삭제
	void OnTurnEnd();

	const TArray<TObjectPtr<UBuffBase>>& GetActiveBuffs() const { return activeBuffs; }

	//버프로 얻는 전투 파생 스탯 보너스 누적, 재계산 덮어쓰기 방지 위해 컴포넌트에 저장
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

	UPROPERTY()
	TArray<TObjectPtr<UBuffBase>> activeBuffs;
};