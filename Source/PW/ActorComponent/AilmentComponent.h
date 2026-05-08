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
	// 새 상태이상 적용 — 스킬에서 적용 대상에게 호출
	// caster는 OnApply에 전달되어 인스턴스 멤버로 스냅샷됨, 컴포넌트는 caster를 저장하지 않음
	void AddAilment(TSubclassOf<UAilmentBase> ailmentClass, ACharacterBase* caster);

	// 인덱스로 상태이상 제거 (만료 처리, 해제 등)
	void RemoveAilmentAt(int32 Index);

	// 턴 시작시 — isStart 상태이상의 Execute 호출
	void OnTurnStart();

	// 턴 종료시 — isEnd 상태이상의 Execute 호출 + 남은 턴 차감 + 만료 정리
	void OnTurnEnd();

	const TArray<TObjectPtr<UAilmentBase>>& GetActiveAilments() const { return activeAilments; }

private:
	UPROPERTY()
	TObjectPtr<ACharacterBase> ownerCharacter = nullptr;

	// 현재 적용 중인 상태이상 인스턴스 목록 — 각 요소는 NewObject로 생성된 per-application 인스턴스
	UPROPERTY()
	TArray<TObjectPtr<UAilmentBase>> activeAilments;
};