// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/BuffComponent.h"
#include "Object/Buff/BuffBase.h"
#include "Characters/CharacterBase.h"

UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();

	ownerCharacter = Cast<ACharacterBase>(GetOwner());
}

void UBuffComponent::AddBuff(TSubclassOf<UBuffBase> buffClass, ACharacterBase* caster)
{
	if (!buffClass || !ownerCharacter) return;

	// CDO에서 isStackable 정책만 미리 확인 — 인스턴스 생성 비용 회피
	const UBuffBase* buffCDO = buffClass.GetDefaultObject();
	if (!buffCDO) return;

	// isStackable=false: 같은 클래스가 이미 걸려있으면 턴수만 누적 (단일 인스턴스 갱신)
	// 스탯 델타는 이미 적용 중이므로 ApplyBuffDelta 재호출하지 않음 (중첩 가산 방지)
	if (!buffCDO->isStackable)
	{
		for (TObjectPtr<UBuffBase>& existing : activeBuffs)
		{
			if (existing && existing->GetClass() == buffClass)
			{
				existing->remainingTurn += buffCDO->buffTurn;
				return;
			}
		}
	}

	// 새 인스턴스 생성 — CDO의 UPROPERTY 기본값을 자동 복사받음
	UBuffBase* buffInstance = NewObject<UBuffBase>(this, buffClass);
	if (!buffInstance) return;

	// caster 스냅샷 — 이 시점 이후 caster가 사라져도 안전
	buffInstance->OnApply(ownerCharacter, caster);
	buffInstance->remainingTurn = buffInstance->buffTurn;

	activeBuffs.Add(buffInstance);

	// 스탯 델타 가산 — OnApply가 인스턴스 스탯 필드를 변조했을 수 있으므로 그 다음 순서
	ownerCharacter->ApplyBuffDelta(buffInstance, +1);
}

void UBuffComponent::RemoveBuffAt(int32 Index)
{
	if (!activeBuffs.IsValidIndex(Index) || !ownerCharacter) return;

	if (UBuffBase* buffInstance = activeBuffs[Index])
	{
		// 스탯 델타 되돌림 — 사라질 때만 검사하는 요구사항 충족
		ownerCharacter->ApplyBuffDelta(buffInstance, -1);
	}
	activeBuffs.RemoveAt(Index);
}

void UBuffComponent::OnTurnStart()
{
	if (!ownerCharacter) return;

	// 턴 시작 효과 — 이 시점에는 만료 처리 X (요구사항: 시작/종료 모두 효과 검사)
	for (UBuffBase* buff : activeBuffs)
	{
		// 직전 효과로 사망했다면 후속 효과 적용 중단 (사망자에게 회복/스탯 변동 적용 방지)
		if (!IsValid(ownerCharacter) || ownerCharacter->IsDead()) return;
		if (buff && buff->isStart)
		{
			buff->Execute(ownerCharacter);
		}
	}
}

void UBuffComponent::OnTurnEnd()
{
	if (!ownerCharacter) return;

	for (UBuffBase* buff : activeBuffs)
	{
		if (!IsValid(ownerCharacter) || ownerCharacter->IsDead()) return;
		if (buff && buff->isEnd)
		{
			buff->Execute(ownerCharacter);
		}
	}

	for (UBuffBase* buff : activeBuffs)
	{
		if (buff)
		{
			buff->remainingTurn--;
		}
	}

	for (int32 i = activeBuffs.Num() - 1; i >= 0; --i)
	{
		if (!activeBuffs[i] || activeBuffs[i]->remainingTurn <= 0)
		{
			RemoveBuffAt(i);
		}
	}
}