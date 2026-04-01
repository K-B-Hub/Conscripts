// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "NavigationSystem.h"
#include "Widget/HealthWidget.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	springArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// 캐릭터 루트에 부착하지 않음 - BattleController Tick에서 월드 위치를 직접 제어
	springArmComponent->TargetArmLength = cameraArmLength;
	springArmComponent->SetRelativeRotation(FRotator(cameraPitchAngle, 0.f, 0.f));
	springArmComponent->bUsePawnControlRotation = false;

	cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	cameraComponent->SetupAttachment(springArmComponent, USpringArmComponent::SocketName);
	cameraComponent->bUsePawnControlRotation = false;

	// 체력 위젯 컴포넌트 — 화면 공간(Screen)으로 설정해 항상 카메라를 향하도록
	healthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	healthWidgetComponent->SetupAttachment(RootComponent);
	healthWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f)); // 캡슐 상단 위
	healthWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	healthWidgetComponent->SetDrawSize(FVector2D(150.f, 20.f));

	// 무기 메시를 오른손 소켓에 부착
	// ⚠️ 소켓 이름을 스켈레톤 에디터에서 만든 이름과 동일하게 맞춰야 함
	WeaponMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMeshComp->SetupAttachment(GetMesh(), FName("WeaponSocket_R"));
	WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	GetCapsuleComponent()->SetCanEverAffectNavigation(true);

	// 캡슐/메시가 스프링암 카메라 충돌 프로브에 걸리지 않도록 Camera 채널 무시
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	// 이동 방향으로 캐릭터가 자동 회전하도록 설정
	bUseControllerRotationYaw = false; // 컨트롤러 회전 비활성화 (OrientToMovement와 충돌)
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// NavOctree 초기 등록은 BattleGameMode::BeginPlay에서 일괄 처리

	// HealthWidget 초기화 — Widget Class는 BP에서 할당
	if (UHealthWidget* HealthWidget = Cast<UHealthWidget>(healthWidgetComponent->GetWidget()))
	{
		HealthWidget->InitHealth(maxHp, hp);
	}
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

void ACharacterBase::ReceiveDamage(int32 Amount)
{
	hp = FMath::Clamp(hp - Amount, 0, maxHp);

	if (UHealthWidget* HealthWidget = Cast<UHealthWidget>(healthWidgetComponent->GetWidget()))
	{
		HealthWidget->ApplyDamage(Amount);
	}

	UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 데미지 %d 수신 → 잔여 체력 %d / %d"), *GetName(), Amount, hp, maxHp);
}

void ACharacterBase::SetNavObstacleEnabled(bool bEnabled)
{
	GetCapsuleComponent()->SetCanEverAffectNavigation(bEnabled);

	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		NavSys->UpdateActorInNavOctree(*this);
	}
}

UStaticMeshComponent* ACharacterBase::GetWeaponMeshComponent() const
{
	return WeaponMeshComp;
}
