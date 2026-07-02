// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AilmentComponent.generated.h"

class UAilmentBase;
class ACharacterBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PW_API UAilmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAilmentComponent();

protected:
	virtual void BeginPlay() override;

public:
	//새 상태이상 적용, 스킬 객체에서 적용 시 호출
	void AddAilment(TSubclassOf<UAilmentBase> ailmentClass, ACharacterBase* caster);

	//인덱스로 제거
	void RemoveAilmentAt(int32 Index);

	//턴 시작시 적용
	void OnTurnStart();

	//턴 종료시 적용 + 만료 삭제
	void OnTurnEnd();

	const TArray<TObjectPtr<UAilmentBase>>& GetActiveAilments() const { return activeAilments; }

private:
	UPROPERTY()
	TObjectPtr<ACharacterBase> ownerCharacter = nullptr;
	
	UPROPERTY()
	TArray<TObjectPtr<UAilmentBase>> activeAilments;
};