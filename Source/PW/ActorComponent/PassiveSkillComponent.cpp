// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/PassiveSkillComponent.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "Characters/CharacterBase.h"
#include "Enum/SkillTypes.h"

UPassiveSkillComponent::UPassiveSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPassiveSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	ownerCharacter = Cast<ACharacterBase>(GetOwner());
}

void UPassiveSkillComponent::AddPassive(TSubclassOf<UPassiveSkillBase> passiveClass)
{
	if (!passiveClass || !ownerCharacter) return;

	UPassiveSkillBase* instance = NewObject<UPassiveSkillBase>(this, passiveClass);
	if (!instance) return;

	instance->SetOwner(ownerCharacter);
	activePassives.Add(instance);

	switch (instance->passiveType)
	{
	case EPassiveType::Stat:
		// 등록 즉시 영구 스탯 가산 — 제거 시까지 유지
		statPassives.Add(instance);
		ownerCharacter->ApplyPassiveStatDelta(instance, +1);
		break;
	case EPassiveType::Reactive:
		reactivePassives.Add(instance);
		// BeforeMove 패시브는 등록 시점에 현재 isMoved 상태로 즉시 동기화
		// (게임 시작 시 미이동 보너스 자동 적용, 전투 중 영입 시에도 현재 상태에 맞춰 적용)
		if (instance->reactiveType == EReactiveType::BeforeMove)
		{
			instance->Execute_BeforeMove(ownerCharacter->IsMoved(), +1);
		}
		break;
	case EPassiveType::Conditional:
		conditionalPassives.Add(instance);
		break;
	}
}

void UPassiveSkillComponent::RemovePassiveAt(int32 Index)
{
	if (!activePassives.IsValidIndex(Index) || !ownerCharacter) return;

	if (UPassiveSkillBase* instance = activePassives[Index])
	{
		switch (instance->passiveType)
		{
		case EPassiveType::Stat:
			ownerCharacter->ApplyPassiveStatDelta(instance, -1);
			statPassives.Remove(instance);
			break;
		case EPassiveType::Reactive:
			// BeforeMove면 현재 적용된 보너스를 명시 revert (대칭성 — 영구 패시브라 사실상 호출 안 됨)
			if (instance->reactiveType == EReactiveType::BeforeMove)
			{
				instance->Execute_BeforeMove(ownerCharacter->IsMoved(), -1);
			}
			reactivePassives.Remove(instance);
			break;
		case EPassiveType::Conditional:
			conditionalPassives.Remove(instance);
			break;
		}
	}
	activePassives.RemoveAt(Index);
}

void UPassiveSkillComponent::DispatchBeforeDamageCalc(ACharacterBase* target,
	int32& dmg, int32& amp, int32& pen, float& acc, float& crit)
{
	for (UPassiveSkillBase* p : reactivePassives)
	{
		if (!p || p->reactiveType != EReactiveType::BeforeDamageCalc) continue;

		if (p->Execute_BeforeDamageCalc(target))
		{
			// 고정 데미지 보너스는 atk 필드를 재사용 (Reactive 컨텍스트에서는 "이 공격에 더할 고정값")
			dmg  += p->atk;
			amp  += p->damageAmplification;
			pen  += p->penetration;
			acc  += p->accuracy;
			crit += p->critical;
		}
	}
}

void UPassiveSkillComponent::DispatchAfterDamage(ACharacterBase* target)
{
	for (UPassiveSkillBase* p : reactivePassives)
	{
		if (!p || p->reactiveType != EReactiveType::AfterDamage) continue;
		p->Execute_AfterDamage(target);
	}
}

void UPassiveSkillComponent::DispatchAfterSlay(const FVector& slainLocation)
{
	for (UPassiveSkillBase* p : reactivePassives)
	{
		if (!p || p->reactiveType != EReactiveType::AfterSlay) continue;
		p->Execute_AfterSlay(slainLocation);
	}
}

void UPassiveSkillComponent::DispatchBeforeMove(bool bIsMoved, int32 sign)
{
	for (UPassiveSkillBase* p : reactivePassives)
	{
		if (!p || p->reactiveType != EReactiveType::BeforeMove) continue;
		p->Execute_BeforeMove(bIsMoved, sign);
	}
}

void UPassiveSkillComponent::RemovePassiveByClass(TSubclassOf<UPassiveSkillBase> passiveClass)
{
	if (!passiveClass) return;

	for (int32 i = 0; i < activePassives.Num(); ++i)
	{
		if (activePassives[i] && activePassives[i]->GetClass() == passiveClass)
		{
			RemovePassiveAt(i);
			return;
		}
	}
}
