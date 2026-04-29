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

	// CDO를 공유 템플릿으로 사용 — 별도 NewObject 하지 않음
	UBuffBase* buffTemplate = buffClass.GetDefaultObject();
	if (!buffTemplate) return;

	// 같은 버프가 이미 존재하면 갱신 — 남은 턴수에 buffTurn을 누적, 시전자도 최신으로 교체
	// 스탯 델타는 이미 적용 중이므로 ApplyBuffDelta 재호출하지 않음 (중첩 가산 방지)
	for (FActiveBuff& existing : activeBuffs)
	{
		if (existing.buff == buffTemplate)
		{
			existing.remainingTurn += buffTemplate->buffTurn;
			existing.caster = caster;
			return;
		}
	}

	// 새 버프 페어 생성 — 스탯 델타는 이 시점에만 캐릭터에 가산
	FActiveBuff active;
	active.buff = buffTemplate;
	active.remainingTurn = buffTemplate->buffTurn;
	active.caster = caster;
	activeBuffs.Add(active);

	ownerCharacter->ApplyBuffDelta(buffTemplate, +1);
}

void UBuffComponent::RemoveBuffAt(int32 Index)
{
	if (!activeBuffs.IsValidIndex(Index) || !ownerCharacter) return;

	UBuffBase* buffTemplate = activeBuffs[Index].buff;
	if (buffTemplate)
	{
		// 스탯 델타 되돌림 — 사라질 때만 검사하는 요구사항 충족
		ownerCharacter->ApplyBuffDelta(buffTemplate, -1);
	}
	activeBuffs.RemoveAt(Index);
}

void UBuffComponent::OnTurnStart()
{
	if (!ownerCharacter) return;

	// 턴 시작 효과 — 이 시점에는 만료 처리 X (요구사항: 시작/종료 모두 효과 검사)
	for (const FActiveBuff& active : activeBuffs)
	{
		if (active.buff && active.buff->isStart)
		{
			active.buff->Execute(ownerCharacter, active.caster.Get());
		}
	}
}

void UBuffComponent::OnTurnEnd()
{
	if (!ownerCharacter) return;

	for (const FActiveBuff& active : activeBuffs)
	{
		if (active.buff && active.buff->isEnd)
		{
			active.buff->Execute(ownerCharacter, active.caster.Get());
		}
	}

	for (FActiveBuff& active : activeBuffs)
	{
		active.remainingTurn--;
	}

	for (int32 i = activeBuffs.Num() - 1; i >= 0; --i)
	{
		if (activeBuffs[i].remainingTurn <= 0)
		{
			RemoveBuffAt(i);
		}
	}
}