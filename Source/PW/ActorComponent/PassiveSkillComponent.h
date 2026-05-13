// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PassiveSkillComponent.generated.h"

class UPassiveSkillBase;
class ACharacterBase;

// 패시브 스킬 보유/관리 컴포넌트
// - Stat: 등록 즉시 ApplyPassiveStatDelta(+1), 제거 시 -1 (영구 적용, 만료 없음)
// - Reactive/Conditional: 이벤트 디스패치 시 매칭되는 인스턴스의 Execute 호출 (추후 구현)
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PW_API UPassiveSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPassiveSkillComponent();

	virtual void BeginPlay() override;
	
	// 패시브 스킬 등록 — NewObject로 인스턴스 생성 후 타입에 따라 처리
	// Stat이면 즉시 스탯 델타 적용, Reactive/Conditional은 목록 보관만
	UPassiveSkillBase* AddPassive(TSubclassOf<UPassiveSkillBase> passiveClass);

	// 인덱스로 패시브 제거 (Stat은 델타 되돌림)
	void RemovePassiveAt(int32 Index);

	// 클래스로 패시브 제거 — 첫 매칭 1건
	void RemovePassiveByClass(TSubclassOf<UPassiveSkillBase> passiveClass);

	const TArray<TObjectPtr<UPassiveSkillBase>>& GetActivePassives() const { return activePassives; }

private:
	UPROPERTY()
	TObjectPtr<ACharacterBase> ownerCharacter = nullptr;

	// 현재 적용 중인 패시브 인스턴스 — 각 요소는 NewObject로 생성된 per-application 인스턴스
	UPROPERTY()
	TArray<TObjectPtr<UPassiveSkillBase>> activePassives;
};
