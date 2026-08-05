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

	//점프 중이면 아치 궤적 추종을 우선 처리
	if (bIsJumping)
	{
		UpdateArcMovement(DeltaTime);
		return;
	}

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

void AAllyCharacterBase::MoveAlongArc(const TArray<FVector>& ArcPoints)
{
	//점프는 시작되면 취소·재시작 불가
	if (bIsJumping) return;
	if (ArcPoints.Num() < 2) return;

	//진행 중이던 걷기 이동은 정리
	bIsMovingToTarget = false;
	pathPoints.Empty();

	//점프도 이동이므로 BeforeMove 전이
	OnMoveStateChanged(true);

	arcPoints = ArcPoints;
	arcIndex = 1; //0은 시작점
	moveDestination = ArcPoints.Last();
	bIsJumping = true;

	//중력·바닥스냅이 궤적을 방해하지 않도록 비행 모드로 전환, 착지 시 복구
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->StopMovementImmediately();
}

void AAllyCharacterBase::UpdateArcMovement(float DeltaTime)
{
	const FVector Cur = GetActorLocation();
	const FVector Target = arcPoints[arcIndex];
	const FVector Delta = Target - Cur;
	const float Step = jumpSpeed * DeltaTime;

	//현재 목표 샘플 도달 시 다음 샘플로, 마지막이면 착지 종료
	if (Delta.Size() <= Step)
	{
		SetActorLocation(Target);
		arcIndex++;
		if (arcIndex >= arcPoints.Num())
		{
			SetActorLocation(moveDestination);
			bIsJumping = false;
			arcPoints.Empty();
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			GetCharacterMovement()->StopMovementImmediately();

			//MoveComplete 패시브를 OnMovementCompleted 브로드캐스트 전에 처리
			if (ABattleGameMode* GM = GetWorld()->GetAuthGameMode<ABattleGameMode>())
			{
				GM->BroadcastMoveComplete();
			}
			OnMovementCompleted.Broadcast();
		}
		return;
	}

	//샘플 방향으로 등속 전진
	SetActorLocation(Cur + Delta.GetSafeNormal() * Step);
}

void AAllyCharacterBase::StopMovement()
{
	bIsMovingToTarget = false;
	pathPoints.Empty();
	GetCharacterMovement()->StopMovementImmediately();
}