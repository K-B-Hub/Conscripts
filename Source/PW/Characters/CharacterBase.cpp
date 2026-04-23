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
	springArmComponent->TargetArmLength = cameraArmLength;
	springArmComponent->SetRelativeRotation(FRotator(cameraPitchAngle, 0.f, 0.f));
	springArmComponent->bUsePawnControlRotation = false;
	springArmComponent->bDoCollisionTest = false;

	cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	cameraComponent->SetupAttachment(springArmComponent, USpringArmComponent::SocketName);
	cameraComponent->bUsePawnControlRotation = false;

	healthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	healthWidgetComponent->SetupAttachment(RootComponent);
	healthWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f)); // 캡슐 상단 위
	healthWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	healthWidgetComponent->SetDrawSize(FVector2D(150.f, 20.f));

	skillInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("SkillInfoWidget"));
	skillInfoWidgetComponent->SetupAttachment(RootComponent);
	skillInfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
	skillInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	skillInfoWidgetComponent->SetDrawSize(FVector2D(200.f, 60.f));
	skillInfoWidgetComponent->SetVisibility(false);

	WeaponMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMeshComp->SetupAttachment(GetMesh(), FName("WeaponSocket_R"));
	WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	skillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));
	if (skillComponent)
	{
		skillComponent->SetOwner(this);
	}

	GetCapsuleComponent()->SetCanEverAffectNavigation(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	bUseControllerRotationYaw = false; 
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (UHealthWidget* HealthWidget = Cast<UHealthWidget>(healthWidgetComponent->GetWidget()))
	{
		HealthWidget->InitHealth(maxHp, hp);
		healthWidget = HealthWidget;
	}
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

void ACharacterBase::GetEXP(bool bIsKill)
{
	if (bIsKill)
	{
		exp += 55;			//추후 경험치 공식 적용 필요
	}
	else
	{
		exp += 20;
	}
	if (exp >= 100)
	{
		exp -= 100;
		LevelUp();
	}
}

void ACharacterBase::LevelUp()
{
	level++;

	if (FMath::RandRange(1, 100) <= hpGrowth)        { maxHp++; hp++; healthWidget->InitHealth(maxHp, hp); }
	if (FMath::RandRange(1, 100) <= atkGrowth)       { atk++; }
	if (FMath::RandRange(1, 100) <= speedGrowth)     { speed++; }
	if (FMath::RandRange(1, 100) <= skillGrowth)     { skill++; }
	if (FMath::RandRange(1, 100) <= defGrowth)       { def++; }
	if (FMath::RandRange(1, 100) <= mentalityGrowth) { mentality++; }

	// 파생 스탯(명중, 회피, 치명) 재계산
	SetDefaultStats();

	UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 레벨업 → Lv.%d"), *GetName(), level);
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
	float hitRoll = FMath::FRandRange(0.f, 100.f);
	if (hitRoll > pendingAccuracy)
	{
		UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 공격 회피"), *GetName());
		return;
	}
	float critRoll = FMath::FRandRange(0.f, 100.f);
	int32 finalDamage = (critRoll <= pendingCritical)
		? FMath::RoundToInt(pendingDMG * 2.f)
		: FMath::RoundToInt(pendingDMG);

	ReceiveDamage(finalDamage);
}

//추후 상태이상 컴포넌트에서 턴 시작시 영향주는 상태이상 적용 및 턴수 감소 필요
void ACharacterBase::InitTurn()
{
	currentActionPoint = actionPoint;
	currentMovingPoint = movingPoint;
}

//추후 상태이상, 버프, 디버프 적용 및 턴수 감소 필요
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

	if (hp <= 0)
	{
		HandleDeath();
	}
}

void ACharacterBase::HandleDeath()
{
	UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 사망"), *GetName());

	OnCharacterDeath.Broadcast(this);

	// NavMesh 장애물 해제
	SetNavObstacleEnabled(false);

	// 충돌 비활성화 — Destroy 전 오버랩 정리 유도
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Destroy();
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
