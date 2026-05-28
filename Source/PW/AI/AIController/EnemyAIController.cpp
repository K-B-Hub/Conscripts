// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/AIController/EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Characters/EnemyBase.h"
#include "Characters/AllyCharacterBase.h"
#include "GameMode/BattleGameMode.h"

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
	// 비전투 상태에서만 감지 평가 — 한 번 전투로 들어가면 단방향
	if (!bIsInCombat)
	{
		EvaluateDetectionAndMaybeSwitch();
	}

	RunCurrentBT();
}

void AEnemyAIController::OnEnemyTurnEnd()
{
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

void AEnemyAIController::EvaluateDetectionAndMaybeSwitch()
{
	AEnemyBase* enemy = Cast<AEnemyBase>(GetPawn());
	if (!enemy) return;

	UWorld* world = GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) return;

	// 시야 시작점은 적 본인의 눈 높이 — APawn::GetPawnViewLocation = ActorLocation + BaseEyeHeight
	const FVector eyeFrom = enemy->GetPawnViewLocation();
	FCollisionQueryParams params(SCENE_QUERY_STAT(EnemyDetection), false, enemy);

	// 타깃 선정은 BT Task가 수행 — 본 함수는 "한 명이라도 감지되면 전투 전환" 트리거 역할만
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

		// 감지 성공 — 단방향 전환 (이후 비전투로 돌아오지 않음)
		bIsInCombat = true;
		if (UBlackboardComponent* bb = GetBlackboardComponent())
		{
			bb->SetValueAsBool(BBKey_InCombat, true);
		}
		UE_LOG(LogTemp, Log, TEXT("[EnemyAIController] %s 감지 → 전투 전환 (트리거 아군: %s)"),
			*enemy->GetName(), *ally->GetName());
		return;
	}
}

void AEnemyAIController::RunCurrentBT()
{
	UBehaviorTree* btToRun = bIsInCombat ? combatBT : nonCombatBT;
	if (!btToRun) return;

	// 이미 같은 BT가 돌고 있으면 RunBehaviorTree는 재시작하지 않고 false 반환 — 정상
	// 전투 전환 직후라면 이전 BT를 멈추고 새 BT 시작
	if (UBehaviorTreeComponent* btComp = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		if (btComp->GetCurrentTree() != btToRun)
		{
			btComp->StopTree();
		}
	}

	RunBehaviorTree(btToRun);
}