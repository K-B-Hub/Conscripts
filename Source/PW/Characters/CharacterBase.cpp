// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "NavigationSystem.h"
#include "../ActorComponent/AilmentComponent.h"
#include "Widget/HealthWidget.h"
#include "Widget/SkillInfoWidget.h"
#include "ActorComponent/SkillComponent.h"
#include "ActorComponent/BuffComponent.h"
#include "Object/Buff/BuffBase.h"
#include "Actorcomponent/AilmentComponent.h"

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

	buffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	ailmentComponent = CreateDefaultSubobject<UAilmentComponent>(TEXT("AilmentComponent"));

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
	if (exp >= maxHp)
	{
		while (exp >= maxHp)
		{
			LevelUp();
			exp -= 100;
		}
	}
}

void ACharacterBase::LevelUp()
{
	level++;

	if (FMath::RandRange(1, 100) <= hpGrowth)
	{
		maxHp++; hp++;
		if (IsValid(healthWidget))
			healthWidget->InitHealth(maxHp, hp);
	}
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
                                     int Penetration, ESkillType SkillType, EPickTeam PickTeam)
{
	pendingSkillType = SkillType;
	if (SkillType == ESkillType::Buff)
	{
		pendingDMG = 0.f;
		// 아군 전용 버프는 회피 차감 없이 스킬 명중을 그대로 사용
		pendingAccuracy = (PickTeam == EPickTeam::AllyOnly) ? Accuracy : (Accuracy - evasion);
		if (pendingAccuracy <= 0)
		{
			pendingAccuracy = 0;
		}
		else if (pendingAccuracy > 100.f)
		{
			pendingAccuracy = 100.f;
		}
		pendingCritical = 0.f;
		return;
	}

	pendingDMG = (Damage * (1 + DamageAmplfication / 100.f) - def * (1 - Penetration / 100.f)) * (1 - damageReduction / 100.f);
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

bool ACharacterBase::ReflectDamage()
{
	if (pendingSkillType == ESkillType::Buff)
	{
		float hitRoll = FMath::FRandRange(0.f, 100.f);
		if (hitRoll > pendingAccuracy)
		{
			UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 버프 회피"), *GetName());
			return false;
		}
		return true;
	}

	float hitRoll = FMath::FRandRange(0.f, 100.f);
	if (hitRoll > pendingAccuracy)
	{
		UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 공격 회피"), *GetName());
		return false;
	}
	float critRoll = FMath::FRandRange(0.f, 100.f);
	int32 finalDamage = (critRoll <= pendingCritical)
		? FMath::RoundToInt(pendingDMG * 2.f)
		: FMath::RoundToInt(pendingDMG);

	ReceiveDamage(finalDamage);
	return true;
}

void ACharacterBase::InitTurn()
{
	currentActionPoint = actionPoint;
	currentMovingPoint = movingPoint;

	if (buffComponent)
	{
		buffComponent->OnTurnStart();
	}
	if (ailmentComponent)
	{
		ailmentComponent->OnTurnStart();
	}
}

void ACharacterBase::EndTurn()
{
	if (buffComponent)
	{
		buffComponent->OnTurnEnd();
	}
	if (ailmentComponent)
	{
		ailmentComponent->OnTurnEnd();
	}
}

void ACharacterBase::ApplyBuffDelta(const UBuffBase* buff, int32 sign)
{
	if (!buff || (sign != 1 && sign != -1)) return;

	//최대 체력 변경 — 현재 체력은 새 max에 클램프
	maxHp += buff->hp * sign;
	if (maxHp <= 0) maxHp = 1;
	hp = FMath::Clamp(hp, 1, maxHp);
	if (healthWidget)
	{
		healthWidget->InitHealth(maxHp, hp);
	}

	//기본 능력치
	atk += buff->atk * sign;
	speed += buff->speed * sign;
	skill += buff->skill * sign;
	def += buff->def * sign;
	movingPoint += buff->movingPoint * sign;
	currentMovingPoint += buff->movingPoint * sign;
	currentMovingPoint = FMath::Clamp(currentMovingPoint, 0, movingPoint);
	mentality += buff->mentality * sign;

	//행동력 — 최대치 변경, 현재치는 새 max에 클램프
	actionPoint += buff->actionPoint * sign;
	currentActionPoint += buff->actionPoint * sign;
	currentActionPoint = FMath::Clamp(currentActionPoint, 0, actionPoint);

	//피해 보정
	damageReduction += buff->damageReduction * sign;
	damageAmplification += buff->damageAmplification * sign;
	penetration += buff->penetration * sign;
	sight += buff->sight * sign;

	//전투 파생 스탯 — 직접 가감 (skill/speed에 의한 자동 재계산은 하지 않음)
	accuracy += buff->accuracy * sign;
	evasion += buff->evasion * sign;
	critical += buff->critical * sign;

	//스킬 측 계산값(damageRatio*atk 등) 재계산 알림 — 사망 상태에서는 스킬 owner가 stale weak ptr이라 의미 없음
	if (skillComponent && !IsDead())
	{
		skillComponent->CalcSkillStats();
	}
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
		InfoWidget->UpdateInfo(pendingDMG, pendingAccuracy, pendingCritical, pendingSkillType);
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
	pendingSkillType = ESkillType::Melee;
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
