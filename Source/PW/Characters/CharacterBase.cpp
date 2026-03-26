// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"

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

	// 무기 메시를 오른손 소켓에 부착
	// ⚠️ 소켓 이름을 스켈레톤 에디터에서 만든 이름과 동일하게 맞춰야 함
	WeaponMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMeshComp->SetupAttachment(GetMesh(), FName("WeaponSocket_R"));
	WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 이동 방향으로 캐릭터가 자동 회전하도록 설정
	bUseControllerRotationYaw = false; // 컨트롤러 회전 비활성화 (OrientToMovement와 충돌)
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
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

int32 ACharacterBase::GetTurnOrder() const
{
	return speed + FMath::RandRange(0, speed);
}

// 추후 상태이상 컴포넌트에서 턴 시작시 영향주는 상태이상 적용 및 턴수 감소 필요
void ACharacterBase::InitTurn()
{
	currentActionPoint = actionPoint;
	currentMovingPoint = movingPoint;
}

// 추후 상태이상, 버프, 디버프 적용 및 턴수 감소 필요
void ACharacterBase::EndTurn()
{
}