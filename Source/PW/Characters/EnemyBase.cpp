// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/EnemyBase.h"
#include "Characters/AllyCharacterBase.h"

void AEnemyBase::HandleDeath()
{
	// 기반 클래스의 lastAttacker를 Ally로 캐스팅 — 경험치 분배는 아군 처치 한정
	AAllyCharacterBase* killer = Cast<AAllyCharacterBase>(lastAttacker.Get());

	UE_LOG(LogTemp, Log, TEXT("[EnemyBase] %s 사망 — 처치자: %s"),
		*GetName(),
		killer ? *killer->GetName() : TEXT("없음"));

	// 경험치 분배 (Destroy 전에 호출해야 유효한 참조 전달)
	OnEnemyDeath.Broadcast(this, killer);

	Super::HandleDeath();
}
