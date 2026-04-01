// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleGameMode.h"
#include "EngineUtils.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "Characters/EnemyBase.h"
#include "PlayerController/BattleController.h"

ABattleGameMode::ABattleGameMode()
{
}

void ABattleGameMode::BeginPlay()
{
	Super::BeginPlay();

	BuildTurnOrder();
	
	for (ACharacterBase* Character : turnOrder)
	{
		if (IsValid(Character))
		{
			Character->SetNavObstacleEnabled(true);
		}
	}

	if (turnOrder.Num() > 0)
	{
		StartCurrentTurn();
	}
}

void ABattleGameMode::BuildTurnOrder()
{
	// 레벨 내 모든 ACharacterBase를 수집
	TArray<TPair<int32, ACharacterBase*>> scored;
	for (TActorIterator<ACharacterBase> It(GetWorld()); It; ++It)
	{
		ACharacterBase* Character = *It;
		if (IsValid(Character))
		{
			// GetTurnOrder()는 RandRange를 포함하므로 한 번만 호출해 캐싱
			scored.Add(TPair<int32, ACharacterBase*>(Character->GetTurnOrder(), Character));
		}
	}

	// 내림차순 정렬 (값이 높을수록 먼저 행동)
	scored.Sort([](const TPair<int32, ACharacterBase*>& A, const TPair<int32, ACharacterBase*>& B)
	{
		return A.Key > B.Key;
	});

	turnOrder.Empty();
	for (const TPair<int32, ACharacterBase*>& Pair : scored)
	{
		turnOrder.Add(Pair.Value);
	}

	UE_LOG(LogTemp, Log, TEXT("[BattleGameMode] 라운드 %d 턴 순서 확정: %d명"), currentRound, turnOrder.Num());
	for (int32 i = 0; i < turnOrder.Num(); ++i)
	{
		UE_LOG(LogTemp, Log, TEXT("  %d. %s"), i + 1, *turnOrder[i]->GetName());
	}
}

void ABattleGameMode::StartCurrentTurn()
{
	if (!turnOrder.IsValidIndex(currentTurnIndex))
	{
		return;
	}

	ACharacterBase* TurnUnit = turnOrder[currentTurnIndex];
	if (!IsValid(TurnUnit))
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[BattleGameMode] 라운드 %d - %s 턴 시작"), currentRound, *TurnUnit->GetName());

	if (AEnemyBase* Enemy = Cast<AEnemyBase>(TurnUnit))
	{
		//Enemy 접근해 AIController의 턴 시작 함수 호출
	}
	else if (AAllyCharacterBase* Ally = Cast<AAllyCharacterBase>(TurnUnit))
	{
		ABattleController* BattleController = Cast<ABattleController>(GetWorld()->GetFirstPlayerController());
		if (BattleController)
		{
			BattleController->Possess(Ally);
			BattleController->InitTurn(Ally);
		}
	}
}

void ABattleGameMode::OnTurnEnd()
{
	currentTurnIndex++;

	// 모든 캐릭터가 행동하면 라운드 종료 → 다음 라운드 시작
	if (currentTurnIndex >= turnOrder.Num())
	{
		currentRound++;
		currentTurnIndex = 0;

		UE_LOG(LogTemp, Log, TEXT("[BattleGameMode] 라운드 %d 시작"), currentRound);

		// 라운드마다 속도 재산정이 필요하면 BuildTurnOrder() 재호출
		BuildTurnOrder();
	}

	StartCurrentTurn();
}