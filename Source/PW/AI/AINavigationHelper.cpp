//Fill out your copyright notice in the Description page of Project Settings.

#include "AI/AINavigationHelper.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Characters/CharacterBase.h"
#include "Actors/Terrain/TerrainBase.h"
#include "GameMode/BattleGameMode.h"

//경로 샘플링 간격(cm), CursorIndicator의 경로 세분화 간격과 맞춰 AI 판정과 플레이어 표시를 일치시킴
static constexpr float TerrainSampleIntervalCm = 10.f;

void UAINavigationHelper::ComputeTerrainPathInfo(const UWorld* World, const TArray<FVector>& PathPoints, FTerrainPathInfo& OutInfo)
{
	OutInfo.WeightedCostCm = 0.f;
	OutInfo.PassedTerrains.Reset();
	OutInfo.DestTerrains.Reset();

	if (PathPoints.Num() < 2) return;

	const ABattleGameMode* gm = World ? World->GetAuthGameMode<ABattleGameMode>() : nullptr;
	const TArray<TObjectPtr<ATerrainBase>> emptyTerrains;
	const TArray<TObjectPtr<ATerrainBase>>& terrains = gm ? gm->GetTerrains() : emptyTerrains;

	//지형이 없으면 경로 길이가 곧 실질 비용
	if (terrains.Num() == 0)
	{
		for (int32 i = 0; i < PathPoints.Num() - 1; ++i)
		{
			OutInfo.WeightedCostCm += FVector::Dist(PathPoints[i], PathPoints[i + 1]);
		}
		return;
	}

	for (int32 i = 0; i < PathPoints.Num() - 1; ++i)
	{
		const FVector& segStart = PathPoints[i];
		const FVector& segEnd = PathPoints[i + 1];
		const float segLength = FVector::Dist(segStart, segEnd);
		if (segLength <= KINDA_SMALL_NUMBER) continue;

		//세그먼트를 균등 분할, 각 조각의 중점에서 지형 배율을 조회
		const int32 stepCount = FMath::Max(1, FMath::CeilToInt(segLength / TerrainSampleIntervalCm));
		const float stepLength = segLength / stepCount;

		for (int32 s = 0; s < stepCount; ++s)
		{
			const float alpha = (s + 0.5f) / stepCount;
			const FVector samplePoint = FMath::Lerp(segStart, segEnd, alpha);

			float multiplier = 1.f;
			for (ATerrainBase* terrain : terrains)
			{
				if (!terrain || !terrain->IsLocationInside(samplePoint)) continue;

				multiplier *= terrain->movingPointCostMultiplier;
				OutInfo.PassedTerrains.AddUnique(terrain);
			}
			OutInfo.WeightedCostCm += stepLength * multiplier;
		}
	}

	//도착 지점 체류 지형, 통과 목록과 별도로 집계
	const FVector& destPoint = PathPoints.Last();
	for (ATerrainBase* terrain : terrains)
	{
		if (terrain && terrain->IsLocationInside(destPoint))
		{
			OutInfo.DestTerrains.AddUnique(terrain);
		}
	}
}

void UAINavigationHelper::GetTerrainsAt(const UWorld* World, const FVector& Location, TArray<TObjectPtr<ATerrainBase>>& OutTerrains)
{
	OutTerrains.Reset();

	const ABattleGameMode* gm = World ? World->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) return;

	for (ATerrainBase* terrain : gm->GetTerrains())
	{
		if (terrain && terrain->IsLocationInside(Location))
		{
			OutTerrains.AddUnique(terrain);
		}
	}
}

bool UAINavigationHelper::CanReach(const ACharacterBase* Mover, const FVector& Target, float& OutPathLengthCm,
	FTerrainPathInfo* OutTerrainInfo)
{
	OutPathLengthCm = 0.f;
	if (OutTerrainInfo) *OutTerrainInfo = FTerrainPathInfo();
	if (!Mover) return false;

	//이동력 단위를 cm로 변환
	const float budgetCm = Mover->GetCurrentMovingPoint() * 100.f;
	if (budgetCm <= 0.f) return false;

	const FVector from = Mover->GetActorLocation();

	//직선거리 초과 시 패스파인딩 생략, 지형 배율은 비용을 늘리기만 하므로 이 조기 반환은 안전
	const float straightCm = FVector::Dist(from, Target);
	if (straightCm > budgetCm) return false;

	UWorld* world = Mover->GetWorld();
	if (!world) return false;

	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(world);
	if (!navSys) return false;

	UNavigationPath* path = navSys->FindPathToLocationSynchronously(world, from, Target);
	//부분 경로는 도달 불가로 처리
	if (!path || !path->IsValid() || path->IsPartial()) return false;

	OutPathLengthCm = path->GetPathLength();

	//지형 배율을 반영한 실질 비용으로 도달 판정, 늪 너머 지점을 도달 가능으로 오판하지 않도록
	FTerrainPathInfo info;
	ComputeTerrainPathInfo(world, path->PathPoints, info);
	if (OutTerrainInfo) *OutTerrainInfo = info;

	return info.WeightedCostCm <= budgetCm;
}

bool UAINavigationHelper::HasLineOfSightFrom(const ACharacterBase* Caster, const FVector& FromLocation, const ACharacterBase* Target)
{
	if (!Caster || !Target) return false;
	UWorld* world = Caster->GetWorld();
	if (!world) return false;

	const FVector eye(0.f, 0.f, 80.f);
	FCollisionQueryParams params(SCENE_QUERY_STAT(AI_LineOfSight), false, Caster);

	FHitResult hit;
	const bool bHit = world->LineTraceSingleByChannel(
		hit,
		FromLocation + eye,
		Target->GetActorLocation() + eye,
		ECC_Visibility,
		params);

	//충돌이 없거나 대상이면 시야선 확보
	return !bHit || hit.GetActor() == Target;
}