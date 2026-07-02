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

private:
	UPROPERTY()
	TObjectPtr<ACharacterBase> ownerCharacter = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<UBuffBase>> activeBuffs;
};