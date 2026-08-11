//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Ailment/Stress/FleeAilment.h"
#include "Characters/AllyCharacterBase.h"
#include "Characters/EnemyBase.h"
#include "GameMode/BattleGameMode.h"
#include "AI/AINavigationHelper.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

//도주 방향이 막혔을 때 틀어보는 최대 각도(도)
static constexpr float FleeJitterDegrees = 20.f;

//도주 목적지 탐색 시도 횟수, 첫 시도는 정반대 직선이고 나머지는 랜덤 각도
static constexpr int32 FleeAttemptCount = 4;

UFleeAilment::UFleeAilment()
{
	ailmentName = NSLOCTEXT("Ailment", "Flee_Name", "생존 본능");
	ailmentDescription = NSLOCTEXT("Ailment", "Flee_Description", "공포에 사로잡혀 통제를 잃는다. 매 턴 가장 가까운 적의 반대 방향으로 이동력을 모두 소진하며 달아난 뒤 턴이 종료된다");

	ailmentTurn = 2;
	isStart = true;
	isEnd = false;
	isStackable = false;
}

void UFleeAilment::Execute(ACharacterBase* affected)
{
	Super::Execute(affected);

	//스트레스 전용 상태이상, 아군 외 대상은 행동 없이 무시
	AAllyCharacterBase* ally = Cast<AAllyCharacterBase>(affected);
	if (!ally) return;

	FVector destination = FVector::ZeroVector;
	if (!PickFleeDestination(ally, destination))
	{
		//갈 곳이 없으면 행동 없이 턴 종료
		ally->RequestEndTurn();
		return;
	}

	UNavigationPath* path = UNavigationSystemV1::FindPathToLocationSynchronously(
		ally->GetWorld(), ally->GetActorLocation(), destination);
	if (!path || path->PathPoints.Num() < 2)
	{
		ally->RequestEndTurn();
		return;
	}

	//이동 자연 종료(도착·이동력 소진 모두 브로드캐스트됨) 시 턴 종료, 재발동 대비 재바인딩
	fleeingAlly = ally;
	ally->OnMovementCompleted.RemoveAll(this);
	ally->OnMovementCompleted.AddUObject(this, &UFleeAilment::OnFleeMoveCompleted);

	ally->MoveAlongPath(path->PathPoints);
}

bool UFleeAilment::PickFleeDestination(AAllyCharacterBase* ally, FVector& outDestination) const
{
	const float budgetCm = ally->GetCurrentMovingPoint() * 100.f;
	if (budgetCm <= 0.f) return false;

	ABattleGameMode* gm = ally->GetWorld()->GetAuthGameMode<ABattleGameMode>();
	if (!gm) return false;

	//가장 가까운 생존 적 탐색, 공황이므로 시야 여부는 따지지 않음
	const FVector allyLoc = ally->GetActorLocation();
	const ACharacterBase* nearest = nullptr;
	float nearestDistSq = TNumericLimits<float>::Max();
	for (AEnemyBase* enemy : gm->GetEnemies())
	{
		if (!enemy || enemy->IsDead()) continue;

		const float distSq = FVector::DistSquared(allyLoc, enemy->GetActorLocation());
		if (distSq < nearestDistSq)
		{
			nearestDistSq = distSq;
			nearest = enemy;
		}
	}
	if (!nearest) return false;

	//최근접 적 반대 방향 일직선, 이동력을 전부 쓰는 지점이 목표
	const FVector away = (allyLoc - nearest->GetActorLocation()).GetSafeNormal2D();
	if (away.IsNearlyZero()) return false;

	//막히면 ±20도 내 랜덤으로 각도만 틀어 재시도, 첫 시도는 정반대 직선
	for (int32 attempt = 0; attempt < FleeAttemptCount; ++attempt)
	{
		const float angleDeg = (attempt == 0) ? 0.f : FMath::FRandRange(-FleeJitterDegrees, FleeJitterDegrees);
		const FVector candidate = allyLoc + away.RotateAngleAxis(angleDeg, FVector::UpVector) * budgetCm;

		float pathLenCm = 0.f;
		FTerrainPathInfo terrainInfo;
		if (UAINavigationHelper::CanReach(ally, candidate, pathLenCm, &terrainInfo)
			&& terrainInfo.WeightedCostCm <= budgetCm)
		{
			outDestination = candidate;
			return true;
		}
	}

	return false;
}

void UFleeAilment::OnFleeMoveCompleted()
{
	if (AAllyCharacterBase* ally = fleeingAlly.Get())
	{
		ally->OnMovementCompleted.RemoveAll(this);
		ally->RequestEndTurn();
	}
}
