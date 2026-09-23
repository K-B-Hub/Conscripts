// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleGameMode.h"
#include "EngineUtils.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "Characters/EnemyBase.h"
#include "PlayerController/BattleController.h"
#include "Pawn/CameraPawn.h"
#include "AI/AIController/EnemyAIController.h"
#include "ActorComponent/PassiveSkillComponent.h"
#include "ActorComponent/BuffComponent.h"
#include "ActorComponent/AilmentComponent.h"
#include "Actors/Terrain/TerrainBase.h"
#include "Enum/SkillTypes.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "Object/Buff/BuffBase.h"
#include "Object/Ailment/AilmentBase.h"
#include "GameInstance/PWGameInstance.h"
#include "DataAsset/StressPoolData.h"
#include "Actors/DeploymentZone.h"
#include "Run/RunProgress.h"
#include "DataAsset/MissionData.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"

ABattleGameMode::ABattleGameMode()
{
	//플레이어는 카메라 전용 폰을 상시 빙의, 캐릭터 빙의 전환 없음
	DefaultPawnClass = ACameraPawn::StaticClass();
}

void ABattleGameMode::BeginPlay()
{
	Super::BeginPlay();

	//모든 액터의 BeginPlay 완료를 보장하기 위해 다음 틱으로 지연
	GetWorldTimerManager().SetTimerForNextTick([this]()
	{
		StartDeployPhase();
	});
}

void ABattleGameMode::StartDeployPhase()
{
	phase = EBattlePhase::Deploy;

	for (TActorIterator<ADeploymentZone> It(GetWorld()); It; ++It)
	{
		deploymentZones.Add(*It);
		It->SetZoneVisible(true);
	}

	//편성을 거치지 않고 전투 맵에 직접 들어온 테스트 상황에서는 배치 단계를 건너뛴다
	const UPWGameInstance* gameInstance = GetGameInstance<UPWGameInstance>();
	const URunProgress* runProgress = gameInstance ? gameInstance->GetRunProgress() : nullptr;
	if (!runProgress || runProgress->GetRoster().Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[BattleGameMode] 로스터 없음 — 배치를 건너뛰고 전투 시작"));
		StartBattlePhase();
		return;
	}

	if (deploymentZones.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleGameMode] 배치 구획이 없어 배치할 수 없습니다"));
	}

	UE_LOG(LogTemp, Log, TEXT("[BattleGameMode] 배치 시작, 인원 %d명 / 구획 %d개"),
		runProgress->GetRoster().Num(), deploymentZones.Num());

	OnDeployPhaseStarted.Broadcast();
}

void ABattleGameMode::StartBattlePhase()
{
	phase = EBattlePhase::Battle;

	//배치가 끝났으므로 구획 표시를 거둔다
	for (ADeploymentZone* zone : deploymentZones)
	{
		if (zone) zone->SetZoneVisible(false);
	}

	{
		//아군/적군 분류 및 사망 델리게이트 바인딩, 턴 순서는 전투 참여자만 담으므로 전 캐릭터를 별도 순회
		for (TActorIterator<ACharacterBase> It(GetWorld()); It; ++It)
		{
			ACharacterBase* Character = *It;
			if (!IsValid(Character)) continue;

			Character->OnCharacterDeath.AddDynamic(this, &ABattleGameMode::OnCharacterDeath);

			if (AAllyCharacterBase* Ally = Cast<AAllyCharacterBase>(Character))
			{
				//아군은 상시 전투로 장애물 등록
				Character->SetNavObstacleEnabled(true);
				allies.Add(Ally);
			}
			else if (AEnemyBase* Enemy = Cast<AEnemyBase>(Character))
			{
				//적은 전투 합류 전까지 미등록, JoinCombat에서 등록
				Character->SetNavObstacleEnabled(false);
				enemies.Add(Enemy);
				Enemy->OnEnemyDeath.AddDynamic(this, &ABattleGameMode::OnEnemyDeath);
			}
		}

		//턴 순서 구성 전에 적 레벨 스케일링, 첫 턴부터 강화 반영
		ApplyEnemyLevelScaling();

		BuildTurnOrder();

		if (turnOrder.Num() > 0)
		{
			//첫 라운드 RoundStart Conditional 패시브, 첫 턴 시작 전 발동
			BroadcastRoundStart();
			StartCurrentTurn();
		}
	}
}

bool ABattleGameMode::IsInsideDeploymentZone(const FVector& point) const
{
	for (const ADeploymentZone* zone : deploymentZones)
	{
		if (zone && zone->ContainsPoint(point)) return true;
	}
	return false;
}

bool ABattleGameMode::DeployAt(int32 rosterIndex, const FVector& location)
{
	if (phase != EBattlePhase::Deploy) return false;
	if (!IsInsideDeploymentZone(location)) return false;

	const UPWGameInstance* gameInstance = GetGameInstance<UPWGameInstance>();
	const URunProgress* runProgress = gameInstance ? gameInstance->GetRunProgress() : nullptr;
	if (!runProgress) return false;

	const TArray<FAllyRunState>& roster = runProgress->GetRoster();
	if (!roster.IsValidIndex(rosterIndex)) return false;

	//이미 배치된 인원이면 옮기기만 한다
	if (const TObjectPtr<AAllyCharacterBase>* existing = deployedAllies.Find(rosterIndex))
	{
		if (IsValid(*existing))
		{
			(*existing)->SetActorLocation(location);
			return true;
		}
	}

	const FAllyRunState& state = roster[rosterIndex];
	if (!state.AllyClass) return false;

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AAllyCharacterBase* ally = GetWorld()->SpawnActor<AAllyCharacterBase>(
		state.AllyClass, location, FRotator::ZeroRotator, params);
	if (!ally) return false;

	//스폰으로 BeginPlay가 끝난 뒤에 복원해야 기본 스킬·패시브 위에 스냅샷이 덮인다
	ally->RestoreRunState(state);
	deployedAllies.Add(rosterIndex, ally);

	UE_LOG(LogTemp, Log, TEXT("[BattleGameMode] 배치: [%d] %s"), rosterIndex, *state.DisplayName);
	return true;
}

bool ABattleGameMode::IsDeployed(int32 rosterIndex) const
{
	const TObjectPtr<AAllyCharacterBase>* found = deployedAllies.Find(rosterIndex);
	return found && IsValid(*found);
}

bool ABattleGameMode::IsDeploymentComplete() const
{
	const UPWGameInstance* gameInstance = GetGameInstance<UPWGameInstance>();
	const URunProgress* runProgress = gameInstance ? gameInstance->GetRunProgress() : nullptr;
	if (!runProgress) return false;

	const int32 rosterNum = runProgress->GetRoster().Num();
	if (rosterNum == 0) return false;

	for (int32 i = 0; i < rosterNum; ++i)
	{
		if (!IsDeployed(i)) return false;
	}
	return true;
}

void ABattleGameMode::ConfirmDeployment()
{
	if (phase != EBattlePhase::Deploy) return;
	if (!IsDeploymentComplete()) return;

	StartBattlePhase();
}

void ABattleGameMode::BuildTurnOrder()
{
	//레벨 내 ACharacterBase 중 턴 대상만 수집
	TArray<TPair<int32, ACharacterBase*>> scored;
	for (TActorIterator<ACharacterBase> It(GetWorld()); It; ++It)
	{
		ACharacterBase* Character = *It;
		if (!IsValid(Character)) continue;

		//비전투 적은 턴 순서 제외, 실시간 BT로 행동하다 전투 합류 시 다음 라운드부터 참여
		if (AEnemyBase* Enemy = Cast<AEnemyBase>(Character))
		{
			AEnemyAIController* aic = Cast<AEnemyAIController>(Enemy->GetController());
			if (!aic || !aic->IsInCombat()) continue;
		}

		//GetTurnOrder()는 RandRange를 포함하므로 한 번만 호출해 캐싱
		scored.Add(TPair<int32, ACharacterBase*>(Character->GetTurnOrder(), Character));
	}

	//내림차순 정렬, 값이 높을수록 먼저 행동
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

	//턴 순서 UI 재생성 통지
	OnTurnOrderRebuilt.Broadcast(turnOrder, currentTurnIndex);
}

int32 ABattleGameMode::GetAverageAllyLevel() const
{
	int32 sum = 0;
	int32 count = 0;
	for (const AAllyCharacterBase* Ally : allies)
	{
		if (!IsValid(Ally)) continue;

		sum += Ally->GetLevel();
		++count;
	}

	//내림으로 처리, 최소 1레벨은 보장
	return (count > 0) ? FMath::Max(1, sum / count) : 1;
}

bool ABattleGameMode::IsSpotClear(const FVector& location, float clearance) const
{
	const float clearanceSq = clearance * clearance;
	for (TActorIterator<ACharacterBase> It(GetWorld()); It; ++It)
	{
		const ACharacterBase* other = *It;
		if (!IsValid(other)) continue;

		//서로 다른 층에 있을 일이 없어 평면 거리로 충분하다
		if (FVector::DistSquared2D(other->GetActorLocation(), location) < clearanceSq) return false;
	}
	return true;
}

bool ABattleGameMode::FindReinforcementSpot(const AAllyCharacterBase* caller, FVector& outLocation) const
{
	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!navSys || !caller) return false;

	const UCapsuleComponent* capsule = caller->GetCapsuleComponent();
	const float radius = capsule->GetScaledCapsuleRadius();
	const float halfHeight = capsule->GetScaledCapsuleHalfHeight();
	const FVector base = caller->GetActorLocation();

	//요청자에 가까운 고리부터 넓혀가며 8방향을 훑는다
	for (int32 ring = 2; ring <= 5; ++ring)
	{
		const float distance = radius * ring;
		for (int32 i = 0; i < 8; ++i)
		{
			const FVector dir = FRotator(0.f, i * 45.f, 0.f).RotateVector(caller->GetActorRightVector());

			FNavLocation projected;
			if (!navSys->ProjectPointToNavigation(base + dir * distance, projected,
				FVector(radius, radius, halfHeight * 2.f))) continue;

			//캡슐 지름만큼은 비어 있어야 스폰이 충돌로 막히지 않는다
			if (!IsSpotClear(projected.Location, radius * 2.f)) continue;

			//NavMesh 지점은 바닥 높이라 캡슐 절반만큼 띄워야 바닥에 끼지 않는다
			outLocation = projected.Location + FVector(0.f, 0.f, halfHeight);
			return true;
		}
	}

	return false;
}

AAllyCharacterBase* ABattleGameMode::JoinReinforcement(TSubclassOf<AAllyCharacterBase> jobClass, const AAllyCharacterBase* caller)
{
	//연속 증원 중 상한에 닿을 수 있어 후보 필터와 별개로 여기서도 막는다
	const UPWGameInstance* gameInstance = GetGameInstance<UPWGameInstance>();
	if (gameInstance && allies.Num() >= gameInstance->GetMaxRosterSize())
	{
		UE_LOG(LogTemp, Log, TEXT("[BattleGameMode] 로스터가 가득 차 증원하지 않습니다"));
		return nullptr;
	}

	FVector spawnLocation;
	if (!jobClass || !caller || !FindReinforcementSpot(caller, spawnLocation))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleGameMode] 증원을 세울 자리를 찾지 못했습니다"));
		return nullptr;
	}

	//평균은 합류 전 기준이라 신병 자신이 평균에 섞이지 않는다
	const int32 targetLevel = GetAverageAllyLevel();

	//빈자리를 골랐어도 지형 굴곡으로 걸릴 수 있어 배치와 같은 보정을 건다
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AAllyCharacterBase* recruit = GetWorld()->SpawnActor<AAllyCharacterBase>(
		jobClass, spawnLocation, caller->GetActorRotation(), params);
	if (!recruit) return nullptr;

	//전투 참여자와 같은 초기화, 사망 시 배열 정리가 신병에게도 걸려야 한다
	recruit->OnCharacterDeath.AddDynamic(this, &ABattleGameMode::OnCharacterDeath);
	recruit->SetNavObstacleEnabled(true);
	allies.Add(recruit);

	//레벨당 대기 강화가 쌓이고, 소비는 신병의 턴 시작이 맡는다
	recruit->ForceLevelUpTo(targetLevel);

	//현재 턴 바로 다음에 끼워 넣어 같은 라운드에 행동시킨다, currentTurnIndex는 밀리지 않는다
	turnOrder.Insert(recruit, FMath::Min(currentTurnIndex + 1, turnOrder.Num()));
	OnTurnOrderRebuilt.Broadcast(turnOrder, currentTurnIndex);

	UE_LOG(LogTemp, Log, TEXT("[BattleGameMode] 증원 합류: %s Lv.%d, 아군 %d명"),
		*recruit->GetName(), recruit->GetLevel(), allies.Num());

	return recruit;
}

void ABattleGameMode::ApplyEnemyLevelScaling()
{
	if (allies.Num() == 0) return;

	const int32 targetLevel = GetAverageAllyLevel() + 2;

	UE_LOG(LogTemp, Log, TEXT("[Upgrade] 적 레벨 스케일링 → 아군 평균+2 = Lv.%d, 적 %d체"), targetLevel, enemies.Num());

	//각 적을 목표 레벨까지 레벨업, 레벨당 랜덤 강화는 EnemyBase가 자동 습득
	for (AEnemyBase* Enemy : enemies)
	{
		if (IsValid(Enemy)) Enemy->ForceLevelUpTo(targetLevel);
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

	//턴 순서 UI 상태 갱신 통지
	OnTurnChanged.Broadcast(currentTurnIndex);

	if (AEnemyBase* Enemy = Cast<AEnemyBase>(TurnUnit))
	{
		//시야 내 적만 카메라 추적, 시야 밖 적 턴은 카메라 무조작
		if (ABattleController* BattleController = Cast<ABattleController>(GetWorld()->GetFirstPlayerController()))
		{
			if (Enemy->IsVisibleToPlayers())
			{
				BattleController->BeginAITurnFollow(Enemy);
			}
			else
			{
				BattleController->ClearAITurnFollow();
			}
		}

		//적 턴 시작, InitTurn 내부에서 AIController에 통지되어 BT 실행
		Enemy->InitTurn();
		return;
	}
	else if (AAllyCharacterBase* Ally = Cast<AAllyCharacterBase>(TurnUnit))
	{
		//빙의 전환 없이 컨트롤러에 턴 유닛만 전달, 카메라는 컨트롤러가 추적 처리
		ABattleController* BattleController = Cast<ABattleController>(GetWorld()->GetFirstPlayerController());
		if (BattleController)
		{
			BattleController->InitTurn(Ally);
		}
	}
}

void ABattleGameMode::OnCharacterDeath(ACharacterBase* DeadCharacter)
{
	//turnOrder에서 제거, currentTurnIndex 보정
	int32 DeadIndex = turnOrder.IndexOfByKey(DeadCharacter);
	bool bActiveUnitDied = false;
	if (DeadIndex != INDEX_NONE)
	{
		turnOrder.RemoveAt(DeadIndex);
		//이미 지나간 인덱스가 제거되면 현재 인덱스 보정
		if (DeadIndex < currentTurnIndex)
		{
			currentTurnIndex--;
		}
		else if (DeadIndex == currentTurnIndex)
		{
			//활성 턴 보유자 사망, 후속 OnTurnEnd가 다음 유닛을 건너뛰지 않도록 사전 감산
			currentTurnIndex--;
			bActiveUnitDied = true;
		}

		//턴 순서 UI 재생성 통지, 활성 턴 사망 시 인덱스는 다음 StartCurrentTurn에서 재보정
		OnTurnOrderRebuilt.Broadcast(turnOrder, FMath::Max(currentTurnIndex, 0));
	}

	//위협 프로파일 정리, 진영 무관
	threatProfiles.Remove(DeadCharacter);

	//아군/적군 배열에서 제거
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

	//AllyDeath/EnemyDeath Conditional 패시브, 캐시 정리 후 생존자에게만 통지
	BroadcastUnitDeath(DeadCharacter);

	//사망 처리 콜 스택 안에서 레벨 전환이 시작되지 않도록 판정을 다음 틱으로 미룬다
	GetWorldTimerManager().SetTimerForNextTick([this]()
	{
		EvaluateMission();
	});

	//활성 턴 보유자가 사망한 경우 콜 스택을 빠져나간 뒤 다음 턴으로 진행
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

//경험치 난이도 보정, 열 인덱스 = EGameDifficulty(스테이지/로그라이크/악몽)
static constexpr float DifficultyExpMod[3] = { 2.f, 1.f, 0.27f };

//레벨 차이(적-아군) 경험치 계수, 행 0=+4 이상 … 행 12=-8 이하, 열 = EGameDifficulty
static constexpr float LevelDiffExpMod[13][3] = {
	{ 2.5f,  1.9f,  1.9f  },	//+4 이상
	{ 2.2f,  1.7f,  1.7f  },	//+3
	{ 1.8f,  1.4f,  1.4f  },	//+2
	{ 1.4f,  1.2f,  1.2f  },	//+1
	{ 1.f,   1.f,   1.f   },	//0
	{ 0.95f, 0.7f,  0.6f  },	//-1
	{ 0.9f,  0.6f,  0.5f  },	//-2
	{ 0.85f, 0.5f,  0.4f  },	//-3
	{ 0.8f,  0.4f,  0.35f },	//-4
	{ 0.7f,  0.35f, 0.3f  },	//-5
	{ 0.65f, 0.3f,  0.25f },	//-6
	{ 0.6f,  0.25f, 0.2f  },	//-7
	{ 0.6f,  0.2f,  0.1f  },	//-8 이하
};

void ABattleGameMode::OnEnemyDeath(AEnemyBase* DeadEnemy, AAllyCharacterBase* Killer)
{
	//GameInstance 미설정 시 스테이지 모드로 간주
	const UPWGameInstance* gameInstance = Cast<UPWGameInstance>(GetGameInstance());
	const int32 difficultyIndex = gameInstance ? static_cast<int32>(gameInstance->GetDifficulty()) : 0;
	const float unitMod = DeadEnemy->IsBoss() ? 2.f : 1.f;

	for (AAllyCharacterBase* Ally : allies)
	{
		if (!IsValid(Ally)) continue;

		//처치자는 55, 나머지 아군은 20
		const float baseExp = (Ally == Killer) ? 55.f : 20.f;
		const int32 levelDiffRow = 4 - FMath::Clamp(DeadEnemy->GetLevel() - Ally->GetLevel(), -8, 4);
		Ally->GainExp(DifficultyExpMod[difficultyIndex] * baseExp * unitMod * LevelDiffExpMod[levelDiffRow][difficultyIndex]);
	}

	UE_LOG(LogTemp, Log, TEXT("[BattleGameMode] %s 처치 → 처치자: %s, 아군 %d명 경험치 획득"),
		*DeadEnemy->GetName(),
		Killer ? *Killer->GetName() : TEXT("없음"),
		allies.Num());
}

const UMissionData* ABattleGameMode::GetCurrentMission() const
{
	const UPWGameInstance* gameInstance = GetGameInstance<UPWGameInstance>();
	const URunProgress* runProgress = gameInstance ? gameInstance->GetRunProgress() : nullptr;
	const FStageEntry* stage = runProgress ? runProgress->GetCurrentStage() : nullptr;

	return stage ? stage->Mission : nullptr;
}

void ABattleGameMode::EvaluateMission()
{
	if (phase != EBattlePhase::Battle) return;

	//아군 전멸은 임무와 무관한 공통 패배 조건
	if (allies.Num() == 0)
	{
		FinishBattle(EBattleResult::Defeat);
		return;
	}

	const UMissionData* mission = GetCurrentMission();
	if (mission && mission->IsComplete(this))
	{
		FinishBattle(EBattleResult::Victory);
	}
}

void ABattleGameMode::FinishBattle(EBattleResult result)
{
	if (phase == EBattlePhase::Result) return;

	//턴 진행을 즉시 멈춘다, 이후 StartCurrentTurn이 돌지 않도록
	phase = EBattlePhase::Result;

	//승리한 경우에만 회수한다. 패배면 allies가 비어 있어 회수할 것이 없다
	if (result == EBattleResult::Victory)
	{
		UPWGameInstance* gameInstance = GetGameInstance<UPWGameInstance>();
		if (URunProgress* runProgress = gameInstance ? gameInstance->GetRunProgress() : nullptr)
		{
			TArray<AAllyCharacterBase*> survivors;
			survivors.Reserve(allies.Num());
			for (const TObjectPtr<AAllyCharacterBase>& ally : allies)
			{
				survivors.Add(ally);
			}
			runProgress->CaptureFromWorld(survivors);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[BattleGameMode] 전투 종료: %s (생존 아군 %d)"),
		result == EBattleResult::Victory ? TEXT("승리") : TEXT("패배"), allies.Num());

	OnBattleFinished.Broadcast(result);
}

void ABattleGameMode::OnTurnEnd()
{
	//승패가 확정된 뒤에는 턴을 더 진행하지 않는다
	if (phase == EBattlePhase::Result) return;

	//턴 기반 목표(생존 N턴 등)를 위해 턴이 넘어갈 때마다 판정
	EvaluateMission();
	if (phase == EBattlePhase::Result) return;

	currentTurnIndex++;

	//모든 캐릭터가 행동하면 라운드 종료, 다음 라운드 시작
	if (currentTurnIndex >= turnOrder.Num())
	{
		currentRound++;
		currentTurnIndex = 0;

		UE_LOG(LogTemp, Log, TEXT("[BattleGameMode] 라운드 %d 시작"), currentRound);

		//라운드마다 속도 재산정이 필요하면 BuildTurnOrder() 재호출
		BuildTurnOrder();

		//라운드 시작 Conditional 패시브, 첫 턴 시작 전에 발동
		BroadcastRoundStart();
	}

	StartCurrentTurn();
}

void ABattleGameMode::RegisterTerrain(ATerrainBase* Terrain)
{
	if (Terrain)
	{
		terrains.AddUnique(Terrain);
	}
}

void ABattleGameMode::UnregisterTerrain(ATerrainBase* Terrain)
{
	terrains.Remove(Terrain);
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

	//각 수신자 기준으로 죽은 대상이 아군/적군인지 판단, 같은 팀은 AllyDeath, 반대 팀은 EnemyDeath
	const bool bDeadIsAlly = Cast<AAllyCharacterBase>(DeadCharacter) != nullptr;

	for (AAllyCharacterBase* Ally : allies)
	{
		if (!IsValid(Ally) || Ally == DeadCharacter || Ally->IsDead()) continue;
		//같은 팀 사망 시 스트레스 30~40, Conditional 패시브가 스트레스 반영 후 상태를 보도록 먼저 부여
		if (bDeadIsAlly)
		{
			Ally->ReceiveStress(FMath::RandRange(30, 40));
		}
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

void ABattleGameMode::ApplyStressEvent(ACharacterBase* Target)
{
	if (!IsValid(Target)) return;

	//풀은 런 단위로 GameInstance에서 조회
	const UPWGameInstance* gameInstance = Cast<UPWGameInstance>(GetGameInstance());
	const UStressPoolData* stressPool = gameInstance ? gameInstance->GetStressPool() : nullptr;
	if (!stressPool) return;

	//20% 긍정, 80% 부정, 긍정 고정 패시브 보유 시 추첨 생략
	const UPassiveSkillComponent* targetPassives = Target->GetPassiveSkillComponent();
	const bool bPositive = (targetPassives && targetPassives->HasStressEventAlwaysPositive())
		|| FMath::RandRange(1, 100) <= 20;
	const TArray<TSubclassOf<UObject>>& pool = bPositive ? stressPool->positiveEvents : stressPool->negativeEvents;
	if (pool.Num() == 0) return;

	UClass* picked = pool[FMath::RandRange(0, pool.Num() - 1)];
	if (!picked) return;

	UE_LOG(LogTemp, Log, TEXT("[BattleGameMode] %s 스트레스 이벤트 발동 → %s '%s' 적용"),
		*Target->GetName(), bPositive ? TEXT("긍정") : TEXT("부정"), *picked->GetName());

	//클래스 계열에 따라 버프/상태이상/패시브로 구분 적용, 시전자는 본인
	if (picked->IsChildOf(UBuffBase::StaticClass()))
	{
		if (UBuffComponent* BC = Target->GetBuffComponent())
		{
			BC->AddBuff(picked, Target);
		}
	}
	else if (picked->IsChildOf(UAilmentBase::StaticClass()))
	{
		if (UAilmentComponent* AC = Target->GetAilmentComponent())
		{
			AC->AddAilment(picked, Target);
		}
	}
	else if (picked->IsChildOf(UPassiveSkillBase::StaticClass()))
	{
		if (UPassiveSkillComponent* PSC = Target->GetPassiveSkillComponent())
		{
			PSC->AddPassive(picked);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleGameMode] 스트레스 이벤트 '%s'는 버프/상태이상/패시브 계열이 아님"), *picked->GetName());
	}
}

const FThreatProfile& ABattleGameMode::GetThreatProfile(const ACharacterBase* Character) const
{
	static const FThreatProfile defaultProfile;
	if (!Character) return defaultProfile;
	const FThreatProfile* found = threatProfiles.Find(Character);
	return found ? *found : defaultProfile;
}

void ABattleGameMode::RecordSkillUse(ACharacterBase* Character, const UActiveSkillBase* Skill)
{
	if (!Character || !Skill) return;

	//위협 모델은 피해 기반, 비-피해 스킬은 스킵 (Heal은 calcDamage가 회복량이라 오염 방지)
	if (Skill->skillType == ESkillType::Buff
		|| Skill->skillType == ESkillType::Ailment
		|| Skill->skillType == ESkillType::Heal)
	{
		return;
	}

	//캐스터 스탯이 이미 반영된 cached 값 사용, CritDamage는 캐스터의 치명타 배율 적용
	const float newDamage    = Skill->calcDamage;
	const float newAccuracy  = Skill->calcAccuracy;
	const float newCritChance= Skill->calcCritical;
	const float newCritDamage= newDamage * Character->GetCriticalDamage();

	const float newHitP  = newAccuracy / 100.f;
	const float newCritP = newCritChance / 100.f;
	const float newExpected = newHitP * ((1.f - newCritP) * newDamage + newCritP * newCritDamage);

	FThreatProfile& prof = threatProfiles.FindOrAdd(Character);

	const float oldHitP  = prof.Accuracy / 100.f;
	const float oldCritP = prof.CritChance / 100.f;
	const float oldExpected = oldHitP * ((1.f - oldCritP) * prof.NormalDamage + oldCritP * prof.CritDamage);

	//기대 피해가 더 크면 가장 강한 스킬로 인정, 사거리 포함 전체 교체
	if (newExpected > oldExpected)
	{
		prof.NormalDamage = newDamage;
		prof.Accuracy     = newAccuracy;
		prof.CritDamage   = newCritDamage;
		prof.CritChance   = newCritChance;
		prof.RangeCm      = Skill->pickRange;

		UE_LOG(LogTemp, Log, TEXT("[BattleGameMode] %s 위협 프로파일 갱신 (기대피해 %.1f, 사거리 %.0f)"),
			*GetNameSafe(Character), newExpected, prof.RangeCm);
	}
}
