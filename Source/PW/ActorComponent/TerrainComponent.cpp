//Fill out your copyright notice in the Description page of Project Settings.

#include "ActorComponent/TerrainComponent.h"
#include "Actors/Terrain/TerrainBase.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/BuffComponent.h"
#include "ActorComponent/AilmentComponent.h"
#include "Object/Buff/BuffBase.h"
#include "Object/Ailment/AilmentBase.h"

UTerrainComponent::UTerrainComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTerrainComponent::BeginPlay()
{
	Super::BeginPlay();

	ownerCharacter = Cast<ACharacterBase>(GetOwner());
}

void UTerrainComponent::EnterTerrain(ATerrainBase* terrain)
{
	if (!terrain || !ownerCharacter || currentTerrains.Contains(terrain)) return;

	currentTerrains.Add(terrain);

	//지형 스탯 델타 적용
	ownerCharacter->ApplyTerrainStatDelta(terrain, 1);

	//진입 시 부여 버프/상태이상, 지형이 시전자
	if (UBuffComponent* BC = ownerCharacter->GetBuffComponent())
	{
		for (const TSubclassOf<UBuffBase>& buffClass : terrain->enterBuffs)
		{
			BC->AddBuff(buffClass, nullptr);
		}
	}
	if (UAilmentComponent* AC = ownerCharacter->GetAilmentComponent())
	{
		for (const TSubclassOf<UAilmentBase>& ailmentClass : terrain->enterAilments)
		{
			AC->AddAilment(ailmentClass, nullptr);
		}
	}

	//파생 클래스의 고유 진입 효과
	terrain->OnCharacterEnter(ownerCharacter);
}

void UTerrainComponent::ExitTerrain(ATerrainBase* terrain)
{
	if (!terrain || !ownerCharacter) return;
	if (currentTerrains.Remove(terrain) == 0) return;

	//파생 고유 효과를 먼저 처리, 스탯 델타 복구 전 상태를 볼 수 있도록
	terrain->OnCharacterExit(ownerCharacter);

	//지형 스탯 델타 복구
	ownerCharacter->ApplyTerrainStatDelta(terrain, -1);
}

void UTerrainComponent::DispatchTurnStart()
{
	if (!ownerCharacter) return;

	for (ATerrainBase* terrain : currentTerrains)
	{
		if (terrain) terrain->OnCharacterTurnStart(ownerCharacter);
	}
}

void UTerrainComponent::DispatchTurnEnd()
{
	if (!ownerCharacter) return;

	for (ATerrainBase* terrain : currentTerrains)
	{
		if (terrain) terrain->OnCharacterTurnEnd(ownerCharacter);
	}
}

float UTerrainComponent::GetMovingPointCostMultiplier() const
{
	float multiplier = 1.f;
	for (ATerrainBase* terrain : currentTerrains)
	{
		if (terrain) multiplier *= terrain->movingPointCostMultiplier;
	}
	return multiplier;
}