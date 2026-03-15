// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = cameraArmLength;

	SpringArmComponent->SetRelativeRotation(FRotator(cameraPitchAngle, 0.f, 0.f));

	SpringArmComponent->bUsePawnControlRotation = false;
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->bInheritRoll = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
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

}

// ─── 이동 인터페이스 ─────────────────────────────────────────────────────────
void ACharacterBase::MoveToLocation(const FVector& DestLocation)
{
	// SimpleMoveToLocation: Controller에 NavMesh 경로 이동 명령 전달
	// 내부적으로 PathFollowingComponent가 경로를 계산하고 이동을 처리함
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), DestLocation);
}

void ACharacterBase::StopMovement()
{
	// 이동 중단: CharacterMovement의 속도를 즉시 0으로 설정
	GetCharacterMovement()->StopMovementImmediately();
}

int32 ACharacterBase::GetTurnOrder() const
{
	return speed + FMath::RandRange(0, speed);
}
