//Fill out your copyright notice in the Description page of Project Settings.

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

	//블랙보드만 미리 초기화
	if (blackboardData)
	{
		UBlackboardComponent* bbComp = nullptr;
		UseBlackboard(blackboardData, bbComp);
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

	//비전투 상태에서만 감지 평가
	if (!bIsInCombat)
	{
		EvaluateDetectionAndMaybeJoinCombat();
	}

	//전투 상태는 UtilityAI로 행동 결정
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
		//컴포넌트 없으면 턴 즉시 종료
		UE_LOG(LogTemp, Warning, TEXT("[EnemyAIController] %s UtilityAIComponent 미부착 — 턴 즉시 종료"),
			*GetNameSafe(GetPawn()));
		OnEnemyTurnEnd();
		return;
	}

	//비전투 BT 실행
	RunCurrentBT();
}

void AEnemyAIController::OnUtilityAITurnComplete()
{
	OnEnemyTurnEnd();
}

void AEnemyAIController::OnEnemyTurnEnd()
{
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

	//다음 틱에 턴 진행
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

	//적 눈높이에서 시야 검사
	const FVector eyeFrom = enemy->GetPawnViewLocation();
	FCollisionQueryParams params(SCENE_QUERY_STAT(EnemyDetection), false, enemy);

	//한 명이라도 감지되면 전투로 전환
	for (AAllyCharacterBase* ally : gm->GetAllies())
	{
		if (!IsValid(ally) || ally->IsDead()) continue;

		//부채꼴 감지 검사
		if (!enemy->IsInDetectionFan(ally->GetActorLocation())) continue;

		//시야 차단 검사
		const FVector eyeTo = ally->GetPawnViewLocation();
		FHitResult hit;
		const bool bHit = world->LineTraceSingleByChannel(hit, eyeFrom, eyeTo, ECC_Visibility, params);
		if (bHit && hit.GetActor() != ally) continue;

		//감지 성공 시 전투 전환
		UE_LOG(LogTemp, Log, TEXT("[EnemyAIController] %s 감지 → 전투 전환 (트리거 아군: %s)"),
			*enemy->GetName(), *ally->GetName());
		JoinCombat(EJoinCombatReason::Detection);
		return;
	}
}

void AEnemyAIController::JoinCombat(EJoinCombatReason Reason)
{
	//이미 전투면 무시
	if (bIsInCombat) return;

	bIsInCombat = true;
	if (UBlackboardComponent* bb = GetBlackboardComponent())
	{
		bb->SetValueAsBool(BBKey_InCombat, true);
	}

	//전투 그룹 등록 위치
	//if (ABattleGameMode* gm = GetWorld()->GetAuthGameMode<ABattleGameMode>())
	//{
	//gm->RegisterCombatant(GetPawn(), Reason);
	//}

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
