// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/AllyCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/BattleGameMode.h"

void AAllyCharacterBase::InitTurn()
{
	Super::InitTurn();

	//대기 중인 레벨업 강화가 있으면 알림, 위젯 생성·잠금은 컨트롤러 책임
	if (pendingUpgradeLevels.Num() > 0)
	{
		OnUpgradeSelectRequested.Broadcast();
	}
}

void AAllyCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsMovingToTarget || pathPoints.Num() == 0) return;

	const FVector CurrentLoc = GetActorLocation();

	//이전 프레임 대비 이동한 거리를 미터로 변환해 이동력 차감, 현재 서 있는 지형의 소모 배율 반영
	const float MovedCm = FVector::Dist(CurrentLoc, lastFrameLocation);
	ConsumeMovingPoint(MovedCm / 100.f * GetTerrainMoveCostMultiplier());
	lastFrameLocation = CurrentLoc;

	//이동력 소진 시 즉시 정지 후 자연 종료 알림
	if (currentMovingPoint <= 0.f)
	{
		StopMovement();
		//MoveComplete 패시브 발동을 OnMovementCompleted 브로드캐스트 전에 처리
		if (ABattleGameMode* GM = GetWorld()->GetAuthGameMode<ABattleGameMode>())
		{
			GM->BroadcastMoveComplete();
		}
		OnMovementCompleted.Broadcast();
		return;
	}

	const FVector Target = pathPoints[pathPointIndex];
	const FVector Delta = Target - CurrentLoc;
	const float Dist2D = FVector2D(Delta.X, Delta.Y).Size();

	//현재 경유점 도달 판정
	if (Dist2D < 5.f)
	{
		pathPointIndex++;

		//마지막 경유점 도달 시 정확한 위치에 스냅 후 이동 종료, 자연 종료 알림
		if (pathPointIndex >= pathPoints.Num())
		{
			SetActorLocation(FVector(moveDestination.X, moveDestination.Y, CurrentLoc.Z));
			GetCharacterMovement()->StopMovementImmediately();
			bIsMovingToTarget = false;
			//MoveComplete 패시브 발동을 OnMovementCompleted 브로드캐스트 전에 처리
			if (ABattleGameMode* GM = GetWorld()->GetAuthGameMode<ABattleGameMode>())
			{
				GM->BroadcastMoveComplete();
			}
			OnMovementCompleted.Broadcast();
			return;
		}
	}

	//다음 경유점 방향으로 입력, 목적지 근접 시 감속
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

void AAllyCharacterBase::HandleDeath()
{
	//이동 중이면 정지
	StopMovement();

	//델리게이트 바인딩 정리, 소멸자에서 정리 시 크래시 방지
	OnMovementCompleted.Clear();

	Super::HandleDeath();
}

void AAllyCharacterBase::MoveAlongPath(const TArray<FVector>& Points)
{
	StopMovement();
	if (Points.Num() == 0) return;

	//실제 이동 시작, BeforeMove 전이
	OnMoveStateChanged(true);

	pathPoints = Points;
	pathPointIndex = 0;
	moveDestination = Points.Last();
	lastFrameLocation = GetActorLocation(); //첫 프레임 거리 오차 방지
	bIsMovingToTarget = true;
}

void AAllyCharacterBase::StopMovement()
{
	bIsMovingToTarget = false;
	pathPoints.Empty();
	GetCharacterMovement()->StopMovementImmediately();
}