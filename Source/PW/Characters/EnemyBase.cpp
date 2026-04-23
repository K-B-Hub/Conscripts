// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/EnemyBase.h"
#include "Characters/AllyCharacterBase.h"

void AEnemyBase::SetLastAttacker(AAllyCharacterBase* Attacker)
{
	if (IsValid(Attacker))
	{
		lastAttacker = Attacker;
	}
}

void AEnemyBase::HandleDeath()
{
	UE_LOG(LogTemp, Log, TEXT("[EnemyBase] %s 사망 — 처치자: %s"),
		*GetName(),
		lastAttacker.IsValid() ? *lastAttacker->GetName() : TEXT("없음"));

	// 경험치 분배 (Destroy 전에 호출해야 유효한 참조 전달)
	OnEnemyDeath.Broadcast(this, lastAttacker.Get());

	Super::HandleDeath();
}
