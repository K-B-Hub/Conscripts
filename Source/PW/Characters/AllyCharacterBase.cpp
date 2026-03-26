// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/AllyCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

void AAllyCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsMovingToTarget || pathPoints.Num() == 0) return;

	const FVector CurrentLoc = GetActorLocation();

	// 이전 프레임 대비 이동한 수평 거리(cm)를 미터로 변환해 currentMovingPoint 차감
	const float MovedCm = FVector::Dist(CurrentLoc, lastFrameLocation);
	currentMovingPoint = FMath::Max(0.f, currentMovingPoint - MovedCm / 100.f);
	lastFrameLocation = CurrentLoc;

	// 이동력 소진 시 즉시 정지
	if (currentMovingPoint <= 0.f)
	{
		StopMovement();
		return;
	}

	const FVector Target = pathPoints[pathPointIndex];
	const FVector Delta = Target - CurrentLoc;
	const float Dist2D = FVector2D(Delta.X, Delta.Y).Size();

	// 현재 경유점 도달 판정 (수평 거리 5cm 이내)
	if (Dist2D < 5.f)
	{
		pathPointIndex++;

		// 마지막 경유점 도달 → 정확한 위치에 스냅 후 이동 종료
		if (pathPointIndex >= pathPoints.Num())
		{
			SetActorLocation(FVector(moveDestination.X, moveDestination.Y, CurrentLoc.Z));
			GetCharacterMovement()->StopMovementImmediately();
			bIsMovingToTarget = false;
			return;
		}
	}

	// 다음 경유점 방향으로 입력 (목적지 근접 시 감속)
	const FVector MoveDir = FVector(Delta.X, Delta.Y, 0.f).GetSafeNormal();

	const float distToDestination = FVector2D(moveDestination.X - CurrentLoc.X,
	                                           moveDestination.Y - CurrentLoc.Y).Size();
	const float moveScale = (distToDestination < moveDecelRadius)
		? FMath::Max(0.15f, distToDestination / moveDecelRadius)
		: 1.0f;

	AddMovementInput(MoveDir, moveScale);
}

void AAllyCharacterBase::EndTurn()
{
	Super::EndTurn();
	StopMovement();
}

void AAllyCharacterBase::MoveAlongPath(const TArray<FVector>& Points)
{
	StopMovement();
	if (Points.Num() == 0) return;

	pathPoints = Points;
	pathPointIndex = 0;
	moveDestination = Points.Last();
	lastFrameLocation = GetActorLocation(); // 첫 프레임 거리 오차 방지
	bIsMovingToTarget = true;
}

void AAllyCharacterBase::StopMovement()
{
	bIsMovingToTarget = false;
	pathPoints.Empty();
	GetCharacterMovement()->StopMovementImmediately();
}