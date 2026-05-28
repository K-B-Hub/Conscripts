// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/AINavigationHelper.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Characters/CharacterBase.h"

bool UAINavigationHelper::CanReach(const ACharacterBase* Mover, const FVector& Target, float& OutPathLengthCm)
{
	OutPathLengthCm = 0.f;
	if (!Mover) return false;

	// 단위 변환: currentMovingPoint(m) → cm
	const float budgetCm = Mover->GetCurrentMovingPoint() * 100.f;
	if (budgetCm <= 0.f) return false;

	const FVector from = Mover->GetActorLocation();

	// 사전 컷 — 직선거리만 봐도 초과면 도달 불가. NavPath는 항상 직선거리 이상이라 거짓 음성 없음.
	const float straightCm = FVector::Dist(from, Target);
	if (straightCm > budgetCm) return false;

	UWorld* world = Mover->GetWorld();
	if (!world) return false;

	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(world);
	if (!navSys) return false;

	UNavigationPath* path = navSys->FindPathToLocationSynchronously(world, from, Target);
	// IsPartial: 목표 미도달 부분 경로 — 도달 불가로 간주
	if (!path || !path->IsValid() || path->IsPartial()) return false;

	OutPathLengthCm = path->GetPathLength();
	return OutPathLengthCm <= budgetCm;
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

	// hit 없음 = 사선 확보. hit이 Target 본인이면 캡슐 충돌이라 통과로 판정 (Detection 패턴과 동일).
	return !bHit || hit.GetActor() == Target;
}