// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleGameMode.h"
#include "EngineUtils.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "Characters/EnemyBase.h"
#include "PlayerController/BattleController.h"
#include "ActorComponent/PassiveSkillComponent.h"
#include "Enum/SkillTypes.h"
#include "Object/Skill/ActiveSkillBase.h"

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
		// 별도 입력이 없으면 카메라가 행동 중인 AI를 추적하도록 컨트롤러에 통지
		if (ABattleController* BattleController = Cast<ABattleController>(GetWorld()->GetFirstPlayerController()))
		{
			BattleController->BeginAITurnFollow(Enemy);
		}

		// 적 턴 시작 — InitTurn 내부에서 AIController에 통지되어 BT 실행, 턴 종료는 BTTask_TurnEnd에서 OnTurnEnd 콜백
		Enemy->InitTurn();
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

const FPlayerThreatProfile& ABattleGameMode::GetPlayerThreatProfile(const AAllyCharacterBase* Ally) const
{
	// 미관측 캐릭터는 디폴트 프로파일 반환 — 초기 보수적 추정으로 동작.
	// CDO를 통해 USTRUCT 디폴트 값(NormalDamage=10, Accuracy=50, ...)에 접근.
	static const FPlayerThreatProfile defaultProfile;
	if (!Ally) return defaultProfile;
	const FPlayerThreatProfile* found = playerThreatProfiles.Find(Ally);
	return found ? *found : defaultProfile;
}

void ABattleGameMode::RecordPlayerSkillUse(AAllyCharacterBase* Ally, const UActiveSkillBase* Skill)
{
	if (!Ally || !Skill) return;

	// 위협 모델은 피해 기반 — 비-피해 스킬은 일단 스킵.
	// (Buff/Ailment에 대한 별도 처리 로직은 추후 추가 여지)
	if (Skill->skillType == ESkillType::Buff
		|| Skill->skillType == ESkillType::Ailment)
	{
		return;
	}

	// 캐스터 스탯이 이미 반영된 cached 값 사용 (SetCalcedStats 결과).
	// CritDamage는 ReflectDamage/PreviewDamage 규약과 동일하게 NormalDamage × 2.
	const float newDamage    = Skill->calcDamage;
	const float newAccuracy  = Skill->calcAccuracy;
	const float newCritChance= Skill->calcCritical;
	const float newCritDamage= newDamage * 2.f;

	const float newHitP  = newAccuracy / 100.f;
	const float newCritP = newCritChance / 100.f;
	const float newExpected = newHitP * ((1.f - newCritP) * newDamage + newCritP * newCritDamage);

	FPlayerThreatProfile& prof = playerThreatProfiles.FindOrAdd(Ally);

	const float oldHitP  = prof.Accuracy / 100.f;
	const float oldCritP = prof.CritChance / 100.f;
	const float oldExpected = oldHitP * ((1.f - oldCritP) * prof.NormalDamage + oldCritP * prof.CritDamage);

	// 기대 피해가 더 크면 가장 강한 스킬로 인정 — 사거리 포함 전체 교체.
	if (newExpected > oldExpected)
	{
		prof.NormalDamage = newDamage;
		prof.Accuracy     = newAccuracy;
		prof.CritDamage   = newCritDamage;
		prof.CritChance   = newCritChance;
		prof.RangeCm      = Skill->pickRange;
	}
}