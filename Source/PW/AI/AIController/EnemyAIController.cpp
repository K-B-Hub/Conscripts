//Fill out your copyright notice in the Description page of Project Settings.

#include "AI/AIController/EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Characters/EnemyBase.h"
#include "Characters/AllyCharacterBase.h"
#include "GameMode/BattleGameMode.h"
#include "AI/UtilityAIComponent.h"
#include "AI/AINavigationHelper.h"
#include "GameFramework/CharacterMovementComponent.h"

const FName AEnemyAIController::BBKey_TargetActor(TEXT("TargetActor"));
const FName AEnemyAIController::BBKey_InCombat(TEXT("InCombat"));

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//블랙보드만 미리 초기화
	if (blackboardData)
	{
		UBlackboardComponent* bbComp = nullptr;
		UseBlackboard(blackboardData, bbComp);
	}

	//비전투는 턴과 무관한 실시간 BT 루프로 행동 시작
	if (!bIsInCombat)
	{
		RunCurrentBT();
	}
}

void AEnemyAIController::OnUnPossess()
{
	//BT 중단
	if (UBehaviorTreeComponent* btComp = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		btComp->StopTree();
	}
	Super::OnUnPossess();
}

void AEnemyAIController::OnEnemyTurnStart()
{
	//자기 충돌로 인한 이동 경로 흔들림 방지
	if (ACharacterBase* pawnAsChar = Cast<ACharacterBase>(GetPawn()))
	{
		pawnAsChar->SetNavObstacleEnabled(false);
	}

	//턴은 전투 참여자만 배정되는 전제, 비전투로 들어오면 즉시 종료
	if (!bIsInCombat)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemyAIController] %s 비전투 상태로 턴 진입 — 턴 순서 필터 확인 필요"),
			*GetNameSafe(GetPawn()));
		OnEnemyTurnEnd();
		return;
	}

	//시야 밖 턴은 연출 딜레이 생략 + 이동 배속으로 빠르게 진행
	AEnemyBase* enemyPawn = Cast<AEnemyBase>(GetPawn());
	bFastForwardTurn = enemyPawn && !enemyPawn->IsVisibleToPlayers();
	if (enemyPawn)
	{
		if (UUtilityAIComponent* ai = enemyPawn->FindComponentByClass<UUtilityAIComponent>())
		{
			ai->SetFastForward(bFastForwardTurn);
		}
		if (bFastForwardTurn)
		{
			if (UCharacterMovementComponent* move = enemyPawn->GetCharacterMovement())
			{
				normalWalkSpeed = move->MaxWalkSpeed;
				move->MaxWalkSpeed *= fastForwardSpeedMultiplier;
			}
		}
	}

	//행동 시작 연출 딜레이, 카메라가 행동 주체를 먼저 비추도록
	if (!bFastForwardTurn && turnStartDelay > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(turnPacingTimerHandle, this,
			&AEnemyAIController::BeginTurnAction, turnStartDelay, false);
		return;
	}
	BeginTurnAction();
}

void AEnemyAIController::BeginTurnAction()
{
	//알람 합류자는 교전 게이트 밖이면 알람 지점 접근만 수행
	if (bHasAlarmFocus)
	{
		if (APawn* pawnPtr = GetPawn())
		{
			if (FVector::Dist(pawnPtr->GetActorLocation(), alarmFocusLocation) <= engagementGateRadius)
			{
				//게이트 통과, 이번 턴부터 UtilityAI로 행동
				bHasAlarmFocus = false;
			}
			else
			{
				//접근 이동 후 턴 종료, 이동 불가면 즉시 종료
				if (!TryStartApproachToAlarm())
				{
					OnEnemyTurnEnd();
				}
				return;
			}
		}
	}

	//전투 상태는 UtilityAI로 행동 결정
	if (APawn* pawn = GetPawn())
	{
		if (UUtilityAIComponent* ai = pawn->FindComponentByClass<UUtilityAIComponent>())
		{
			ai->OnTurnComplete.Clear();
			ai->OnTurnComplete.AddUObject(this, &AEnemyAIController::OnUtilityAITurnComplete);
			ai->ExecuteTurn();
			return;
		}
	}
	//컴포넌트 없으면 턴 즉시 종료
	UE_LOG(LogTemp, Warning, TEXT("[EnemyAIController] %s UtilityAIComponent 미부착 — 턴 즉시 종료"),
		*GetNameSafe(GetPawn()));
	OnEnemyTurnEnd();
}

void AEnemyAIController::OnUtilityAITurnComplete()
{
	OnEnemyTurnEnd();
}

void AEnemyAIController::OnEnemyTurnEnd()
{
	//고속 턴 종료, 이동 배속 복원
	const bool bWasFastForward = bFastForwardTurn;
	if (bWasFastForward)
	{
		bFastForwardTurn = false;
		if (ACharacterBase* pawnAsChar = Cast<ACharacterBase>(GetPawn()))
		{
			if (UCharacterMovementComponent* move = pawnAsChar->GetCharacterMovement())
			{
				move->MaxWalkSpeed = normalWalkSpeed;
			}
		}
	}

	//NavMesh 장애물 복원 및 버프/상태이상 차감, TurnEnd Conditional 패시브 발동
	if (ACharacterBase* pawnAsChar = Cast<ACharacterBase>(GetPawn()))
	{
		pawnAsChar->SetNavObstacleEnabled(true);
		pawnAsChar->EndTurn();
	}

	//다음 턴에 BT를 처음부터 재실행
	if (UBehaviorTreeComponent* btComp = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		btComp->StopTree(EBTStopMode::Safe);
	}
	
	if (UBlackboardComponent* bb = GetBlackboardComponent())
	{
		bb->SetValueAsObject(BBKey_TargetActor, nullptr);
	}

	//턴 종료 연출 딜레이 후 다음 턴 진행
	UWorld* world = GetWorld();
	if (!world) return;

	TWeakObjectPtr<AEnemyAIController> weakSelf(this);
	FTimerDelegate advanceTurn = FTimerDelegate::CreateLambda([weakSelf]()
	{
		if (!weakSelf.IsValid()) return;
		if (ABattleGameMode* gm = weakSelf->GetWorld()->GetAuthGameMode<ABattleGameMode>())
		{
			gm->OnTurnEnd();
		}
	});
	if (!bWasFastForward && turnEndDelay > 0.f)
	{
		world->GetTimerManager().SetTimer(turnPacingTimerHandle, advanceTurn, turnEndDelay, false);
	}
	else
	{
		world->GetTimerManager().SetTimerForNextTick(advanceTurn);
	}
}

void AEnemyAIController::EvaluateDetectionAndMaybeJoinCombat()
{
	AEnemyBase* enemy = Cast<AEnemyBase>(GetPawn());
	if (!enemy) return;

	UWorld* world = GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) return;

	//감지 대상은 전투 중인 캐릭터, 플레이어 진영은 항상 전투중 판정
	TArray<ACharacterBase*> combatants;
	for (AAllyCharacterBase* ally : gm->GetAllies())
	{
		if (IsValid(ally) && !ally->IsDead())
		{
			combatants.Add(ally);
		}
	}
	//전투 중인 같은 진영을 목격해도 전투 합류
	for (AEnemyBase* other : gm->GetEnemies())
	{
		if (!IsValid(other) || other == enemy || other->IsDead()) continue;
		AEnemyAIController* otherAIC = Cast<AEnemyAIController>(other->GetController());
		if (otherAIC && otherAIC->IsInCombat())
		{
			combatants.Add(other);
		}
	}

	//적 눈높이에서 시야 검사
	const FVector eyeFrom = enemy->GetPawnViewLocation();
	FCollisionQueryParams params(SCENE_QUERY_STAT(EnemyDetection), false, enemy);

	//한 명이라도 감지되면 전투로 전환
	for (ACharacterBase* target : combatants)
	{
		//부채꼴 감지 검사
		if (!enemy->IsInDetectionFan(target->GetActorLocation())) continue;

		//시야 차단 검사
		const FVector eyeTo = target->GetPawnViewLocation();
		FHitResult hit;
		const bool bHit = world->LineTraceSingleByChannel(hit, eyeFrom, eyeTo, ECC_Visibility, params);
		if (bHit && hit.GetActor() != target) continue;

		//감지 성공 시 전투 전환
		UE_LOG(LogTemp, Log, TEXT("[EnemyAIController] %s 감지 → 전투 전환 (트리거: %s)"),
			*enemy->GetName(), *target->GetName());
		JoinCombat(EJoinCombatReason::Detection);
		return;
	}
}

void AEnemyAIController::JoinCombat(EJoinCombatReason Reason, const FVector& AlarmOrigin)
{
	//이미 전투면 무시
	if (bIsInCombat) return;

	bIsInCombat = true;

	//알람 합류는 게이트 통과 전까지 알람 지점을 접근 목표로 저장
	if (Reason == EJoinCombatReason::Alarm)
	{
		alarmFocusLocation = AlarmOrigin;
		bHasAlarmFocus = true;
	}
	if (UBlackboardComponent* bb = GetBlackboardComponent())
	{
		bb->SetValueAsBool(BBKey_InCombat, true);
	}

	//실시간 비전투 BT와 진행 중 이동 중단, 턴은 다음 라운드부터 배정됨
	if (UBehaviorTreeComponent* btComp = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		btComp->StopTree(EBTStopMode::Safe);
	}
	StopMovement();

	//전투 합류와 함께 NavMesh 장애물 등록, 이후 본인 턴에만 해제
	if (ACharacterBase* pawnChar = Cast<ACharacterBase>(GetPawn()))
	{
		pawnChar->SetNavObstacleEnabled(true);
	}

	//전투 그룹 등록 위치
	//if (ABattleGameMode* gm = GetWorld()->GetAuthGameMode<ABattleGameMode>())
	//{
	//gm->RegisterCombatant(GetPawn(), Reason);
	//}

	UE_LOG(LogTemp, Log, TEXT("[EnemyAIController] %s JoinCombat (Reason=%d)"),
		*GetNameSafe(GetPawn()), static_cast<int32>(Reason));

	//감지·피격 전환만 근접 합류 파동 전파, 파동·알람 합류자는 재전파하지 않음
	if (Reason == EJoinCombatReason::Detection || Reason == EJoinCombatReason::Attacked)
	{
		BroadcastProximityWave();
	}
}

void AEnemyAIController::BroadcastProximityWave()
{
	AEnemyBase* enemy = Cast<AEnemyBase>(GetPawn());
	UWorld* world = GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!enemy || !gm) return;

	const FVector origin = enemy->GetActorLocation();
	const float radius = enemy->GetProximityWaveRadius();
	if (radius <= 0.f) return;

	int32 joined = 0;
	for (AEnemyBase* other : gm->GetEnemies())
	{
		if (!IsValid(other) || other == enemy || other->IsDead()) continue;
		//파동은 소리라 시야 차단 무시, 거리로만 판정
		if (FVector::Dist(other->GetActorLocation(), origin) > radius) continue;

		if (AEnemyAIController* otherAIC = Cast<AEnemyAIController>(other->GetController()))
		{
			if (!otherAIC->IsInCombat())
			{
				otherAIC->JoinCombat(EJoinCombatReason::Proximity);
				++joined;
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[EnemyAIController] %s 근접 합류 파동 (반경 %.0f, 합류 %d명)"),
		*enemy->GetName(), radius, joined);
}

bool AEnemyAIController::TryStartApproachToAlarm()
{
	ACharacterBase* pawnChar = Cast<ACharacterBase>(GetPawn());
	if (!pawnChar) return false;

	const float budgetCm = pawnChar->GetCurrentMovingPoint() * 100.f;
	if (budgetCm <= 0.f) return false;

	const FVector pawnLoc = pawnChar->GetActorLocation();
	const FVector desired = ComputeApproachDestination(pawnLoc, budgetCm);
	if (FVector::Dist(pawnLoc, desired) < 1.f) return false;

	//직선 목적지가 막혀 있으면 비율 축소 재시도
	static const float ratios[] = { 1.f, 0.7f, 0.4f };
	FVector dest = pawnLoc;
	float pathLenCm = 0.f;
	bool bFound = false;
	for (float ratio : ratios)
	{
		const FVector candidate = pawnLoc + (desired - pawnLoc) * ratio;
		if (UAINavigationHelper::CanReach(pawnChar, candidate, pathLenCm) && pathLenCm <= budgetCm)
		{
			dest = candidate;
			bFound = true;
			break;
		}
	}
	if (!bFound) return false;

	//중복 콜백 방지
	ReceiveMoveCompleted.RemoveDynamic(this, &AEnemyAIController::OnApproachMoveCompleted);
	ReceiveMoveCompleted.AddUniqueDynamic(this, &AEnemyAIController::OnApproachMoveCompleted);

	FAIMoveRequest req(dest);
	req.SetUsePathfinding(true);
	req.SetAcceptanceRadius(10.f);

	if (MoveTo(req) != EPathFollowingRequestResult::RequestSuccessful)
	{
		ReceiveMoveCompleted.RemoveDynamic(this, &AEnemyAIController::OnApproachMoveCompleted);
		return false;
	}

	//이동 상태 반영 및 이동력 차감
	pawnChar->OnMoveStateChanged(true);
	pawnChar->ConsumeMovingPoint(pathLenCm / 100.f);

	UE_LOG(LogTemp, Log, TEXT("[EnemyAIController] %s 알람 지점 접근 이동 (잔여 거리 %.0f)"),
		*GetNameSafe(pawnChar), FVector::Dist(dest, alarmFocusLocation));
	return true;
}

FVector AEnemyAIController::ComputeApproachDestination(const FVector& PawnLoc, float BudgetCm) const
{
	//TODO: 알람 지점(alarmFocusLocation) 방향 목적지 결정
	//선택지 1: 이동력(BudgetCm)만큼 최대 직진 — 게이트 안쪽 깊숙이 진입 가능
	//선택지 2: 게이트 경계(engagementGateRadius) 근처에서 정지 — 경계에 도열 후 다음 턴부터 UtilityAI
	//반환 Z는 PawnLoc.Z 유지, 도달 가능 여부는 호출부가 검증
	FVector targetLoc = alarmFocusLocation;
	targetLoc.Z = PawnLoc.Z;

	const FVector direction = (targetLoc - PawnLoc).GetSafeNormal();
	if (direction.IsNearlyZero()) return PawnLoc;

	return PawnLoc + direction * BudgetCm;
}

void AEnemyAIController::OnApproachMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	ReceiveMoveCompleted.RemoveDynamic(this, &AEnemyAIController::OnApproachMoveCompleted);

	//이동 완료 패시브 발동
	if (Result == EPathFollowingResult::Success)
	{
		if (ABattleGameMode* gm = GetWorld()->GetAuthGameMode<ABattleGameMode>())
		{
			gm->BroadcastMoveComplete();
		}
	}
	OnEnemyTurnEnd();
}

void AEnemyAIController::RunCurrentBT()
{
	if (!nonCombatBT) return;

	//같은 BT가 실행 중이면 재시작하지 않음
	if (UBehaviorTreeComponent* btComp = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		if (btComp->GetCurrentTree() != nonCombatBT)
		{
			btComp->StopTree();
		}
	}

	RunBehaviorTree(nonCombatBT);
}
