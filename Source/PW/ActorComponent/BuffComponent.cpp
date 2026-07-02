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

	//CDO로 중첩 가능한지만 확인
	const UBuffBase* buffCDO = buffClass.GetDefaultObject();
	if (!buffCDO) return;

	//중첩 불가일 경우 턴수만 조정
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

	//중첩 가능일경우 새 객체 생성
	UBuffBase* buffInstance = NewObject<UBuffBase>(this, buffClass);
	if (!buffInstance) return;

	//caster NPE 방지 위해 caster 스냅
	buffInstance->OnApply(ownerCharacter, caster);
	buffInstance->remainingTurn = buffInstance->buffTurn;

	activeBuffs.Add(buffInstance);

	//버프 적용
	ownerCharacter->ApplyBuffDelta(buffInstance, +1);
}

void UBuffComponent::RemoveBuffAt(int32 Index)
{
	if (!activeBuffs.IsValidIndex(Index) || !ownerCharacter) return;

	if (UBuffBase* buffInstance = activeBuffs[Index])
	{
		//버프 값 초기화
		ownerCharacter->ApplyBuffDelta(buffInstance, -1);
	}
	activeBuffs.RemoveAt(Index);
}

void UBuffComponent::OnTurnStart()
{
	if (!ownerCharacter) return;

	//턴 시작 고유 효과 발동
	for (UBuffBase* buff : activeBuffs)
	{
		//직전 사망시 예외처리
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
		if (buff)
		{
			if (buff->isEnd)
			{
				//종료시 효과 발동
				buff->Execute(ownerCharacter);
			}
			//턴 수 차감
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