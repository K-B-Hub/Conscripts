// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/AilmentComponent.h"
#include "Object/Ailment/AilmentBase.h"
#include "Characters/CharacterBase.h"

UAilmentComponent::UAilmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAilmentComponent::BeginPlay()
{
	Super::BeginPlay();

	ownerCharacter = Cast<ACharacterBase>(GetOwner());
}

void UAilmentComponent::AddAilment(TSubclassOf<UAilmentBase> ailmentClass, ACharacterBase* caster)
{
	if (!ailmentClass || !ownerCharacter) return;

	// CDO에서 isStackable 정책만 미리 확인
	const UAilmentBase* ailmentCDO = ailmentClass.GetDefaultObject();
	if (!ailmentCDO) return;

	// isStackable=false: 같은 클래스가 이미 걸려있으면 턴수만 누적 (단일 인스턴스 갱신)
	if (!ailmentCDO->isStackable)
	{
		for (TObjectPtr<UAilmentBase>& existing : activeAilments)
		{
			if (existing && existing->GetClass() == ailmentClass)
			{
				existing->remainingTurn += ailmentCDO->ailmentTurn;
				return;
			}
		}
	}

	// 새 인스턴스 생성 — CDO의 UPROPERTY 기본값을 자동 복사받음
	UAilmentBase* ailmentInstance = NewObject<UAilmentBase>(this, ailmentClass);
	if (!ailmentInstance) return;

	// caster 스냅샷 — 이 시점 이후 caster가 사라져도 안전
	ailmentInstance->OnApply(ownerCharacter, caster);
	ailmentInstance->remainingTurn = ailmentInstance->ailmentTurn;

	activeAilments.Add(ailmentInstance);
}

void UAilmentComponent::RemoveAilmentAt(int32 Index)
{
	if (!activeAilments.IsValidIndex(Index)) return;

	// AilmentBase는 stat delta가 없으므로 ApplyBuffDelta 같은 되돌림 호출 불필요
	activeAilments.RemoveAt(Index);
}

void UAilmentComponent::OnTurnStart()
{
	if (!ownerCharacter) return;

	for (UAilmentBase* ailment : activeAilments)
	{
		// 직전 효과로 사망했다면 후속 효과 적용 중단
		if (!IsValid(ownerCharacter) || ownerCharacter->IsDead()) return;
		if (ailment && ailment->isStart)
		{
			ailment->Execute(ownerCharacter);
		}
	}
}

void UAilmentComponent::OnTurnEnd()
{
	if (!ownerCharacter) return;

	for (UAilmentBase* ailment : activeAilments)
	{
		if (!IsValid(ownerCharacter) || ownerCharacter->IsDead()) return;
		if (ailment && ailment->isEnd)
		{
			ailment->Execute(ownerCharacter);
		}
	}

	for (UAilmentBase* ailment : activeAilments)
	{
		if (ailment)
		{
			ailment->remainingTurn--;
		}
	}

	for (int32 i = activeAilments.Num() - 1; i >= 0; --i)
	{
		if (!activeAilments[i] || activeAilments[i]->remainingTurn <= 0)
		{
			RemoveAilmentAt(i);
		}
	}
}