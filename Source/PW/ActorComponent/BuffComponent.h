// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"

class UBuffBase;
class ACharacterBase;

// 캐릭터에게 적용 중인 버프 1개의 런타임 상태
// BuffBase 자체는 클래스당 CDO 하나가 공유 템플릿이므로,
// 남은 턴수와 시전자처럼 인스턴스마다 달라지는 정보는 여기에 저장
USTRUCT(BlueprintType)
struct FActiveBuff
{
	GENERATED_BODY()

	// 공유 버프 템플릿 (CDO 포인터, 수정 금지)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBuffBase> buff = nullptr;

	// 이 캐릭터 기준으로 남은 적용 턴수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 remainingTurn = 0;

	// 이 버프를 건 캐릭터 — 도트 피해/회복 등 시전자 스탯 기반 효과용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TWeakObjectPtr<ACharacterBase> caster;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PW_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBuffComponent();

protected:
	virtual void BeginPlay() override;

public:
	// 새 버프 적용 — 스킬에서 적용 대상에게 호출
	// buffClass의 CDO를 공유 템플릿으로 참조, 남은 턴수/시전자만 새 페어로 보관
	void AddBuff(TSubclassOf<UBuffBase> buffClass, ACharacterBase* caster);

	// 인덱스로 버프 제거 (만료 처리, 디스펠 등)
	// 스탯 델타 되돌림 후 배열에서 제거
	void RemoveBuffAt(int32 Index);

	// 턴 시작시 — isStart 버프의 Execute 호출
	void OnTurnStart();

	// 턴 종료시 — isEnd 버프의 Execute 호출 + 남은 턴 차감 + 만료 정리
	void OnTurnEnd();

	const TArray<FActiveBuff>& GetActiveBuffs() const { return activeBuffs; }

private:
	UPROPERTY()
	TObjectPtr<ACharacterBase> ownerCharacter = nullptr;

	// 현재 적용 중인 버프 목록
	UPROPERTY()
	TArray<FActiveBuff> activeBuffs;
};