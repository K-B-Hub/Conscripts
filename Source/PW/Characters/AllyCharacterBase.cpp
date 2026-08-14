// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/AllyCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/BattleGameMode.h"

void AAllyCharacterBase::InitTurn()
{
	Super::InitTurn();

	//대기 중인 레벨업 강화가 있으면 알림, 위젯 생성·잠금은 컨트롤러 책임
	//턴 강제 종료 예약·상태이상 점유 시 보류, 대기 큐가 유지되어 다음 행동 가능한 턴에 다시 표시됨
	if (pendingUpgradeLevels.Num() > 0 && !bTurnEndRequested && !bAilmentDrivenTurn)
	{
		OnUpgradeSelectRequested.Broadcast();
	}
}

void AAllyCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//아치 궤적 추종 중이면 걷기 이동 처리 생략 (추종은 CharacterBase::Tick이 담당)
	if (bIsArcMoving) return;

	if (!bIsMovingToTarget || pathPoints.Num() == 0) return;

	const FVector CurrentLoc = GetActorLocation();

	//이전 프레임 대비 이동한 거리를 미터로 변환해 이동력 차감, 지형 배율·포복 자세 배율 반영
	const float MovedCm = FVector::Dist(CurrentLoc, lastFrameLocation);
	ConsumeMovingPoint(MovedCm / 100.f * GetTerrainMoveCostMultiplier() * GetStanceMoveCostMultiplier());
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

void AAllyCharacterBase::MoveAlongArc(const TArray<FVector>& ArcPoints, bool bForced)
{
	if (bIsArcMoving) return;

	//진행 중이던 걷기 이동은 정리
	bIsMovingToTarget = false;
	pathPoints.Empty();

	Super::MoveAlongArc(ArcPoints, bForced);
}

void AAllyCharacterBase::NotifyArcMoveCompleted()
{
	OnMovementCompleted.Broadcast();
}

void AAllyCharacterBase::StopMovement()
{
	bIsMovingToTarget = false;
	pathPoints.Empty();
	GetCharacterMovement()->StopMovementImmediately();
}