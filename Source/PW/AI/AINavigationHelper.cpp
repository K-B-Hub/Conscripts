//Fill out your copyright notice in the Description page of Project Settings.

#include "AI/AINavigationHelper.h"
#include "PW.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Components/CapsuleComponent.h"
#include "Characters/CharacterBase.h"
#include "Actors/Terrain/TerrainBase.h"
#include "GameMode/BattleGameMode.h"

//경로 샘플링 간격(cm), CursorIndicator의 경로 세분화 간격과 맞춰 AI 판정과 플레이어 표시를 일치시킴
static constexpr float TerrainSampleIntervalCm = 10.f;

//후보 지점 바닥 투영의 수직 탐색 반경(cm)
static constexpr float NavFloorSnapHeightCm = 10.f * CmPerMeter;

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
	const float budgetCm = Mover->GetCurrentMovingPoint() * CmPerMeter;
	if (budgetCm <= 0.f) return false;

	const FVector from = Mover->GetNavAgentLocation();

	//직선거리 초과 시 패스파인딩 생략, 지형 배율은 비용을 늘리기만 하므로 이 조기 반환은 안전
	//발밑과 캡슐 중심의 높이차가 섞이지 않도록 평면 거리
	const float straightCm = FVector::Dist2D(from, Target);
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

bool UAINavigationHelper::SnapToNavFloor(const ACharacterBase* Mover, FVector& InOutLocation)
{
	if (!Mover) return false;
	UWorld* world = Mover->GetWorld();
	UNavigationSystemV1* navSys = world ? UNavigationSystemV1::GetCurrent(world) : nullptr;
	if (!navSys) return false;

	//수평은 기본 투영 범위, 수직은 내리막·오르막 후보를 모두 잡도록 넓게
	FVector extent = navSys->GetDefaultSupportedAgentConfig().DefaultQueryExtent;
	extent.Z = NavFloorSnapHeightCm;

	FNavLocation projected;
	if (!navSys->ProjectPointToNavigation(InOutLocation, projected, extent)) return false;

	//후보는 캡슐 중심 기준이라 반높이만큼 올림
	InOutLocation = projected.Location
		+ FVector(0.f, 0.f, Mover->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	return true;
}

bool UAINavigationHelper::FindBudgetPointToward(const ACharacterBase* Mover, const FVector& Goal, FVector& OutDest, float& OutCostCm)
{
	if (!Mover) return false;
	const float budgetCm = Mover->GetCurrentMovingPoint() * CmPerMeter;
	if (budgetCm <= 0.f) return false;

	UWorld* world = Mover->GetWorld();
	UNavigationSystemV1* navSys = world ? UNavigationSystemV1::GetCurrent(world) : nullptr;
	if (!navSys) return false;

	//목표가 캐릭터면 발밑이 통행 불가 구멍 안이라 넓게 투영해 가장자리를 목표로
	FVector goalExtent = navSys->GetDefaultSupportedAgentConfig().DefaultQueryExtent;
	goalExtent.X = goalExtent.Y = 2.f * CmPerMeter;
	FNavLocation goal;
	if (!navSys->ProjectPointToNavigation(Goal, goal, goalExtent)) return false;

	//막혀 있어도 갈 수 있는 데까지 가도록 부분 경로 허용
	const FVector from = Mover->GetNavAgentLocation();
	UNavigationPath* path = navSys->FindPathToLocationSynchronously(world, from, goal.Location);
	if (!path || !path->IsValid() || path->PathPoints.Num() < 2) return false;
	const TArray<FVector>& pts = path->PathPoints;

	//경로를 따라 lengthCm 지점, 경로보다 길면 끝점
	auto pointAlongPath = [&pts](float lengthCm) -> FVector
	{
		for (int32 i = 0; i < pts.Num() - 1; ++i)
		{
			const float seg = FVector::Dist(pts[i], pts[i + 1]);
			if (lengthCm <= seg) return FMath::Lerp(pts[i], pts[i + 1], seg > 0.f ? lengthCm / seg : 0.f);
			lengthCm -= seg;
		}
		return pts.Last();
	};

	//이동력 길이에서 자르고, 지형 배율로 비용이 넘치면 조금씩 당긴다
	static const float ratios[] = { 1.f, 0.75f, 0.5f, 0.25f };
	for (float ratio : ratios)
	{
		const FVector dest = pointAlongPath(budgetCm * ratio);
		if (FVector::Dist2D(from, dest) < 0.1f * CmPerMeter) return false;

		float pathLenCm = 0.f;
		FTerrainPathInfo info;
		if (CanReach(Mover, dest, pathLenCm, &info))
		{
			OutDest = dest;
			OutCostCm = info.WeightedCostCm;
			return true;
		}
	}
	return false;
}

bool UAINavigationHelper::HasLineOfSightFrom(const ACharacterBase* Caster, const FVector& FromLocation, const ACharacterBase* Target)
{
	if (!Caster || !Target) return false;
	UWorld* world = Caster->GetWorld();
	if (!world) return false;

	//caster·target 각자의 자세 눈높이 적용, 포복이면 낮은 엄폐물에 트레이스가 막힘
	const FVector casterEye(0.f, 0.f, Caster->GetEyeHeightZ());
	const FVector targetEye(0.f, 0.f, Target->GetEyeHeightZ());
	FCollisionQueryParams params(SCENE_QUERY_STAT(AI_LineOfSight), false, Caster);

	FHitResult hit;
	const bool bHit = world->LineTraceSingleByChannel(
		hit,
		FromLocation + casterEye,
		Target->GetActorLocation() + targetEye,
		ECC_Visibility,
		params);

	//충돌이 없거나 대상이면 시야선 확보
	return !bHit || hit.GetActor() == Target;
}
