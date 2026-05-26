// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleGameMode.h"
#include "EngineUtils.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "Characters/EnemyBase.h"
#include "PlayerController/BattleController.h"
#include "ActorComponent/PassiveSkillComponent.h"
#include "Enum/SkillTypes.h"

ABattleGameMode::ABattleGameMode()
{
}

void ABattleGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 모든 액터의 BeginPlay 완료를 보장하기 위해 첫 턴 시작을 다음 틱으로 지연
	GetWorldTimerManager().SetTimerForNextTick([this]()
	{
		BuildTurnOrder();

		// 아군/적군 분류 및 적 사망 델리게이트 바인딩
		for (ACharacterBase* Character : turnOrder)
		{
			if (!IsValid(Character)) continue;

			Character->SetNavObstacleEnabled(true);
			Character->OnCharacterDeath.AddDynamic(this, &ABattleGameMode::OnCharacterDeath);

			if (AAllyCharacterBase* Ally = Cast<AAllyCharacterBase>(Character))
			{
				allies.Add(Ally);
			}
			else if (AEnemyBase* Enemy = Cast<AEnemyBase>(Character))
			{
				enemies.Add(Enemy);
				Enemy->OnEnemyDeath.AddDynamic(this, &ABattleGameMode::OnEnemyDeath);
			}
		}

		if (turnOrder.Num() > 0)
		{
			// 첫 라운드 RoundStart Conditional 패시브 — 첫 턴 시작 전 발동
			BroadcastRoundStart();
			StartCurrentTurn();
		}
	});
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
		// 적군 AI 미구현 — 턴 스킵
		OnTurnEnd();
		return;
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

void ABattleGameMode::OnCharacterDeath(ACharacterBase* DeadCharacter)
{
	// turnOrder에서 제거 — currentTurnIndex 보정
	int32 DeadIndex = turnOrder.IndexOfByKey(DeadCharacter);
	bool bActiveUnitDied = false;
	if (DeadIndex != INDEX_NONE)
	{
		turnOrder.RemoveAt(DeadIndex);
		// 이미 지나간 인덱스가 제거되면 현재 인덱스 보정
		if (DeadIndex < currentTurnIndex)
		{
			currentTurnIndex--;
		}
		else if (DeadIndex == currentTurnIndex)
		{
			// 활성 턴 보유자 사망 — RemoveAt이 다음 유닛을 같은 인덱스로 당겼으므로
			// 후속 OnTurnEnd의 ++가 그 유닛을 건너뛰지 않도록 사전 감산
			currentTurnIndex--;
			bActiveUnitDied = true;
		}
	}

	// 아군/적군 배열에서 제거
	if (AAllyCharacterBase* Ally = Cast<AAllyCharacterBase>(DeadCharacter))
	{
		allies.Remove(Ally);
	}
	else if (AEnemyBase* Enemy = Cast<AEnemyBase>(DeadCharacter))
	{
		enemies.Remove(Enemy);
	}

	UE_LOG(LogTemp, Log, TEXT("[BattleGameMode] %s 전장에서 제거 (잔여: 아군 %d, 적 %d)"),
		*DeadCharacter->GetName(), allies.Num(), enemies.Num());

	// AllyDeath/EnemyDeath Conditional 패시브 — 캐시 정리 후 생존자에게만 통지
	// (DeadCharacter는 아직 Destroy 전이라 Cast<AAllyCharacterBase> 가능)
	BroadcastUnitDeath(DeadCharacter);

	// 활성 턴 보유자가 InitTurn/EndTurn 도중 사망 — 콜 스택을 빠져나간 뒤 다음 턴으로 진행
	// (BattleController가 dead pawn을 possess 중일 수 있어 정리/전환을 한 틱 미룸)
	if (bActiveUnitDied)
	{
		GetWorldTimerManager().SetTimerForNextTick([this]()
		{
			if (ABattleController* BC = Cast<ABattleController>(GetWorld()->GetFirstPlayerController()))
			{
				BC->EndTurn();
			}
			OnTurnEnd();
		});
	}
}

void ABattleGameMode::OnEnemyDeath(AEnemyBase* DeadEnemy, AAllyCharacterBase* Killer)
{
	for (AAllyCharacterBase* Ally : allies)
	{
		if (!IsValid(Ally)) continue;

		// 처치자는 true, 나머지 아군은 false
		Ally->GetEXP(Ally == Killer);
	}

	UE_LOG(LogTemp, Log, TEXT("[BattleGameMode] %s 처치 → 처치자: %s, 아군 %d명 경험치 획득"),
		*DeadEnemy->GetName(),
		Killer ? *Killer->GetName() : TEXT("없음"),
		allies.Num());
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

		// 라운드 시작 Conditional 패시브 — 첫 턴 시작 전에 발동
		BroadcastRoundStart();
	}

	StartCurrentTurn();
}

void ABattleGameMode::BroadcastRoundStart()
{
	for (ACharacterBase* Character : turnOrder)
	{
		if (!IsValid(Character) || Character->IsDead()) continue;
		if (UPassiveSkillComponent* PSC = Character->GetPassiveSkillComponent())
		{
			PSC->DispatchConditional(EConditionalType::RoundStart);
		}
	}
}

void ABattleGameMode::BroadcastMoveComplete()
{
	for (ACharacterBase* Character : turnOrder)
	{
		if (!IsValid(Character) || Character->IsDead()) continue;
		if (UPassiveSkillComponent* PSC = Character->GetPassiveSkillComponent())
		{
			PSC->DispatchConditional(EConditionalType::MoveComplete);
		}
	}
}

void ABattleGameMode::BroadcastUnitDeath(ACharacterBase* DeadCharacter)
{
	if (!DeadCharacter) return;

	// 각 수신자 기준으로 죽은 대상이 아군/적군인지 판단
	// 같은 팀 배열에 속한 생존자 → AllyDeath, 반대 팀 배열 → EnemyDeath
	const bool bDeadIsAlly = Cast<AAllyCharacterBase>(DeadCharacter) != nullptr;

	for (AAllyCharacterBase* Ally : allies)
	{
		if (!IsValid(Ally) || Ally == DeadCharacter || Ally->IsDead()) continue;
		if (UPassiveSkillComponent* PSC = Ally->GetPassiveSkillComponent())
		{
			PSC->DispatchConditional(bDeadIsAlly ? EConditionalType::AllyDeath : EConditionalType::EnemyDeath);
		}
	}

	for (AEnemyBase* Enemy : enemies)
	{
		if (!IsValid(Enemy) || Enemy == DeadCharacter || Enemy->IsDead()) continue;
		if (UPassiveSkillComponent* PSC = Enemy->GetPassiveSkillComponent())
		{
			PSC->DispatchConditional(bDeadIsAlly ? EConditionalType::EnemyDeath : EConditionalType::AllyDeath);
		}
	}
}