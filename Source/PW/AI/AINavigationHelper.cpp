//Fill out your copyright notice in the Description page of Project Settings.

#include "AI/AINavigationHelper.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Characters/CharacterBase.h"

bool UAINavigationHelper::CanReach(const ACharacterBase* Mover, const FVector& Target, float& OutPathLengthCm)
{
	OutPathLengthCm = 0.f;
	if (!Mover) return false;

	//이동력 단위를 cm로 변환
	const float budgetCm = Mover->GetCurrentMovingPoint() * 100.f;
	if (budgetCm <= 0.f) return false;

	const FVector from = Mover->GetActorLocation();

	//직선거리 초과 시 패스파인딩 생략
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

	//충돌이 없거나 대상이면 시야선 확보
	return !bHit || hit.GetActor() == Target;
}
