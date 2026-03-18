// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	springArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	springArmComponent->SetupAttachment(RootComponent);
	springArmComponent->TargetArmLength = cameraArmLength;

	springArmComponent->SetRelativeRotation(FRotator(cameraPitchAngle, 0.f, 0.f));

	springArmComponent->bUsePawnControlRotation = false;
	springArmComponent->bInheritPitch = false;
	springArmComponent->bInheritYaw = false;
	springArmComponent->bInheritRoll = false;

	cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	cameraComponent->SetupAttachment(springArmComponent, USpringArmComponent::SocketName);
	cameraComponent->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsMovingToTarget || pathPoints.Num() == 0) return;

	const FVector CurrentLoc = GetActorLocation();
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

	// 다음 경유점 방향으로 입력
	const FVector MoveDir = FVector(Delta.X, Delta.Y, 0.f).GetSafeNormal();
	AddMovementInput(MoveDir, 1.0f);
}

// ─── 이동 인터페이스 ─────────────────────────────────────────────────────────
void ACharacterBase::MoveAlongPath(const TArray<FVector>& Points)
{
	StopMovement();
	if (Points.Num() == 0) return;

	pathPoints = Points;
	pathPointIndex = 0;
	moveDestination = Points.Last();
	bIsMovingToTarget = true;
}

void ACharacterBase::StopMovement()
{
	bIsMovingToTarget = false;
	pathPoints.Empty();
	GetCharacterMovement()->StopMovementImmediately();
}

int32 ACharacterBase::GetTurnOrder() const
{
	return speed + FMath::RandRange(0, speed);
}