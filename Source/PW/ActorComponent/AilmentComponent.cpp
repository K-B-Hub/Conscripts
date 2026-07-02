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

	//CDO에서 isStackable만 먼저  확인
	const UAilmentBase* ailmentCDO = ailmentClass.GetDefaultObject();
	if (!ailmentCDO) return;

	//중첩 불가능 시 턴수만 갱신
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

	//중첩 가능 시 새 객체 생성
	UAilmentBase* ailmentInstance = NewObject<UAilmentBase>(this, ailmentClass);
	if (!ailmentInstance) return;

	//시전자가 죽어 포인터가 사라질 수 있으니 값 스냅해놓음
	ailmentInstance->OnApply(ownerCharacter, caster);
	ailmentInstance->remainingTurn = ailmentInstance->ailmentTurn;

	activeAilments.Add(ailmentInstance);
}

void UAilmentComponent::RemoveAilmentAt(int32 Index)
{
	if (!activeAilments.IsValidIndex(Index)) return;
	
	activeAilments.RemoveAt(Index);
}

void UAilmentComponent::OnTurnStart()
{
	if (!ownerCharacter) return;

	for (UAilmentBase* ailment : activeAilments)
	{
		//직전 효과로 사망했다면 중단
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
		//직전 효과로 사망했다면 중단
		if (!IsValid(ownerCharacter) || ownerCharacter->IsDead()) return;
		if (ailment)
		{
			if (ailment->isEnd)
			{
				//턴 종료시 효과 발동
				ailment->Execute(ownerCharacter);
			}
			//턴 수 차감
			ailment->remainingTurn--;
		}
	}

	//잔여 턴수 없을시 삭제
	for (int32 i = activeAilments.Num() - 1; i >= 0; --i)
	{
		if (!activeAilments[i] || activeAilments[i]->remainingTurn <= 0)
		{
			RemoveAilmentAt(i);
		}
	}
}