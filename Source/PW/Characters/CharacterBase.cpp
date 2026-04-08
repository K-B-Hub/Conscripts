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
#include "Widget/SkillInfoWidget.h"
#include "ActorComponent/SkillComponent.h"

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

	// 스킬 전투 예측 위젯 — 체력 위젯 위에 표시, 기본 비활성
	skillInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("SkillInfoWidget"));
	skillInfoWidgetComponent->SetupAttachment(RootComponent);
	skillInfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
	skillInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	skillInfoWidgetComponent->SetDrawSize(FVector2D(200.f, 60.f));
	skillInfoWidgetComponent->SetVisibility(false);

	// 무기 메시를 오른손 소켓에 부착
	// ⚠️ 소켓 이름을 스켈레톤 에디터에서 만든 이름과 동일하게 맞춰야 함
	WeaponMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMeshComp->SetupAttachment(GetMesh(), FName("WeaponSocket_R"));
	WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// 스킬 컴포넌트
	skillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));
	if (skillComponent)
	{
		skillComponent->SetOwner(this);
	}

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
	// 스킬 등록 후 스탯 계산 — BeginPlay에서 호출해야 파생 클래스 override가 실행됨
	SetDefaultStats();
	SetDefaultSkills();
}

void ACharacterBase::SetDefaultSkills()
{
	//파생 클래스에서 구현
}

void ACharacterBase::SetDefaultStats()
{
	accuracy = skill * 1.2;
	evasion = speed * 1.2;
	critical = skill * 0.5;
	if (skillComponent)
	{
		skillComponent->CalcSkillStats();
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

void ACharacterBase::ReduceActionPoint(int32 amount)
{
	currentActionPoint = FMath::Clamp(currentActionPoint - amount, 0, actionPoint);
}

void ACharacterBase::ReduceBattleResource(int32 amount)
{
	battleResource = FMath::Clamp(battleResource - amount, 0, battleResource);
}

void ACharacterBase::CalculateDamage(float Damage, float Accuracy, float Critical, int32 DamageAmplfication,
                                     int Penetration)
{
	pendingDMG = (Damage * (1 + DamageAmplfication / 100) - def * (1 - Penetration / 100)) * (1 - damageReduction / 100);
	if (pendingDMG <= 0)
	{
		pendingDMG = 0;
	}
	pendingAccuracy = Accuracy - evasion;
	if (pendingAccuracy <= 0)
	{
		pendingAccuracy = 0;
	}
	else if (pendingAccuracy > 100.f)
	{
		pendingAccuracy = 100.f;
	}
	pendingCritical = Critical;
}

void ACharacterBase::ReflectDamage()
{
	// 명중 판정
	float hitRoll = FMath::FRandRange(0.f, 100.f);
	if (hitRoll > pendingAccuracy)
	{
		UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 공격 회피"), *GetName());
		return;
	}

	// 크리티컬 판정 (크리티컬 시 2배 데미지)
	float critRoll = FMath::FRandRange(0.f, 100.f);
	int32 finalDamage = (critRoll <= pendingCritical)
		? FMath::RoundToInt(pendingDMG * 2.f)
		: FMath::RoundToInt(pendingDMG);

	ReceiveDamage(finalDamage);
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

void ACharacterBase::ShowSkillInfo()
{
	skillInfoWidgetComponent->SetVisibility(true);
	if (USkillInfoWidget* InfoWidget = Cast<USkillInfoWidget>(skillInfoWidgetComponent->GetWidget()))
	{
		InfoWidget->UpdateInfo(pendingDMG, pendingAccuracy, pendingCritical);
	}
}

void ACharacterBase::HideSkillInfo()
{
	skillInfoWidgetComponent->SetVisibility(false);
}

void ACharacterBase::ClearPendingDamage()
{
	pendingDMG = 0.f;
	pendingAccuracy = 0.f;
	pendingCritical = 0.f;
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
