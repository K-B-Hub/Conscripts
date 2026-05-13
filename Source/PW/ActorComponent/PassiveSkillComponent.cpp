// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/PassiveSkillComponent.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "Characters/CharacterBase.h"

UPassiveSkillComponent::UPassiveSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPassiveSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	ownerCharacter = Cast<ACharacterBase>(GetOwner());
}

UPassiveSkillBase* UPassiveSkillComponent::AddPassive(TSubclassOf<UPassiveSkillBase> passiveClass)
{
	if (!passiveClass || !ownerCharacter) return nullptr;

	UPassiveSkillBase* instance = NewObject<UPassiveSkillBase>(this, passiveClass);
	if (!instance) return nullptr;

	instance->SetOwner(ownerCharacter);
	activePassives.Add(instance);

	// Stat 패시브는 등록 즉시 영구 스탯 가산 — 제거 시까지 유지
	if (instance->passiveType == EPassiveType::Stat)
	{
		ownerCharacter->ApplyPassiveStatDelta(instance, +1);
	}

	return instance;
}

void UPassiveSkillComponent::RemovePassiveAt(int32 Index)
{
	if (!activePassives.IsValidIndex(Index) || !ownerCharacter) return;

	if (UPassiveSkillBase* instance = activePassives[Index])
	{
		if (instance->passiveType == EPassiveType::Stat)
		{
			ownerCharacter->ApplyPassiveStatDelta(instance, -1);
		}
	}
	activePassives.RemoveAt(Index);
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
