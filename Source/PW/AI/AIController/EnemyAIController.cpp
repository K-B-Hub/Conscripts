// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/AIController/EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Characters/EnemyBase.h"
#include "Characters/AllyCharacterBase.h"
#include "GameMode/BattleGameMode.h"
#include "AI/UtilityAIComponent.h"

const FName AEnemyAIController::BBKey_TargetActor(TEXT("TargetActor"));
const FName AEnemyAIController::BBKey_InCombat(TEXT("InCombat"));

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 블랙보드만 미리 초기화 — BT 실행은 적 턴 시작 시에만 진행
	if (blackboardData)
	{
		UBlackboardComponent* bbComp = nullptr;
		UseBlackboard(blackboardData, bbComp);
	}
}

void AEnemyAIController::OnUnPossess()
{
	// BT 중단 — Destroy/사망 시 안전망
	if (UBehaviorTreeComponent* btComp = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		btComp->StopTree();
	}
	Super::OnUnPossess();
}

void AEnemyAIController::OnEnemyTurnStart()
{
	// 자기 자신이 NavMesh 장애물로 등록되어 있으면 MoveTo가 경로를 못 잡거나 짧게 잡아 진동.
	// BattleController.cpp:72의 플레이어 측 패턴과 동일. OnEnemyTurnEnd에서 복원.
	if (ACharacterBase* pawnAsChar = Cast<ACharacterBase>(GetPawn()))
	{
		pawnAsChar->SetNavObstacleEnabled(false);
	}

	// 비전투 상태에서만 감지 평가 — 한 번 전투로 들어가면 단방향
	if (!bIsInCombat)
	{
		EvaluateDetectionAndMaybeJoinCombat();
	}

	// 전투 진입 시 — UtilityAI가 행동 결정. BT는 사용 안 함.
	if (bIsInCombat)
	{
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
		// 컴포넌트 미부착 시 안전망 — 턴 즉시 종료
		UE_LOG(LogTemp, Warning, TEXT("[EnemyAIController] %s UtilityAIComponent 미부착 — 턴 즉시 종료"),
			*GetNameSafe(GetPawn()));
		OnEnemyTurnEnd();
		return;
	}

	// 비전투 — nonCombatBT만 실행
	RunCurrentBT();
}

void AEnemyAIController::OnUtilityAITurnComplete()
{
	OnEnemyTurnEnd();
}

void AEnemyAIController::OnEnemyTurnEnd()
{
	// 턴 종료 — NavMesh 장애물로 다시 등록 (다른 캐릭터 경로 계산 시 본인을 우회하게)
	if (ACharacterBase* pawnAsChar = Cast<ACharacterBase>(GetPawn()))
	{
		pawnAsChar->SetNavObstacleEnabled(true);
	}

	// BT 즉시 정지 — 다음 턴 시작 시 RunBehaviorTree가 루트부터 새로 실행되도록
	if (UBehaviorTreeComponent* btComp = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		btComp->StopTree(EBTStopMode::Safe);
	}
	
	if (UBlackboardComponent* bb = GetBlackboardComponent())
	{
		bb->SetValueAsObject(BBKey_TargetActor, nullptr);
	}

	// GameMode 턴 진행은 다음 틱에 — BT ExecuteTask 콜스택 내에서 다음 적의 BT가 즉시 시작되는 재진입 회피
	UWorld* world = GetWorld();
	if (!world) return;

	TWeakObjectPtr<AEnemyAIController> weakSelf(this);
	world->GetTimerManager().SetTimerForNextTick([weakSelf]()
	{
		if (!weakSelf.IsValid()) return;
		if (ABattleGameMode* gm = weakSelf->GetWorld()->GetAuthGameMode<ABattleGameMode>())
		{
			gm->OnTurnEnd();
		}
	});
}

void AEnemyAIController::EvaluateDetectionAndMaybeJoinCombat()
{
	AEnemyBase* enemy = Cast<AEnemyBase>(GetPawn());
	if (!enemy) return;

	UWorld* world = GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) return;

	// 시야 시작점은 적 본인의 눈 높이 — APawn::GetPawnViewLocation = ActorLocation + BaseEyeHeight
	const FVector eyeFrom = enemy->GetPawnViewLocation();
	FCollisionQueryParams params(SCENE_QUERY_STAT(EnemyDetection), false, enemy);

	//한 명이라도 감지되면 전투로 전환
	for (AAllyCharacterBase* ally : gm->GetAllies())
	{
		if (!IsValid(ally) || ally->IsDead()) continue;

		// 1차: 부채꼴 (거리 + 각도) — EnemyBase가 책임
		if (!enemy->IsInDetectionFan(ally->GetActorLocation())) continue;

		// 2차: 시야 차단 — 엄폐/벽 사이에 있으면 감지 실패
		// hit이 발생했고 그 actor가 ally가 아니면 차단된 것으로 판정
		const FVector eyeTo = ally->GetPawnViewLocation();
		FHitResult hit;
		const bool bHit = world->LineTraceSingleByChannel(hit, eyeFrom, eyeTo, ECC_Visibility, params);
		if (bHit && hit.GetActor() != ally) continue;

		// 감지 성공 — 단일 진입점으로 위임 (단방향 전환)
		UE_LOG(LogTemp, Log, TEXT("[EnemyAIController] %s 감지 → 전투 전환 (트리거 아군: %s)"),
			*enemy->GetName(), *ally->GetName());
		JoinCombat(EJoinCombatReason::Detection);
		return;
	}
}

void AEnemyAIController::JoinCombat(EJoinCombatReason Reason)
{
	// 단방향 — 이미 전투면 no-op
	if (bIsInCombat) return;

	bIsInCombat = true;
	if (UBlackboardComponent* bb = GetBlackboardComponent())
	{
		bb->SetValueAsBool(BBKey_InCombat, true);
	}

	// 추후 GameMode 전투 그룹 등록·이니셔티브 삽입 진입점 — 현재는 비워둠
	// if (ABattleGameMode* gm = GetWorld()->GetAuthGameMode<ABattleGameMode>())
	// {
	//     gm->RegisterCombatant(GetPawn(), Reason);
	// }

	UE_LOG(LogTemp, Log, TEXT("[EnemyAIController] %s JoinCombat (Reason=%d)"),
		*GetNameSafe(GetPawn()), static_cast<int32>(Reason));
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
