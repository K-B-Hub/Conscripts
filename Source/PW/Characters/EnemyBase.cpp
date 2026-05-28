// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/EnemyBase.h"
#include "Characters/AllyCharacterBase.h"
#include "AI/AIController/EnemyAIController.h"
#include "AI/UtilityAIComponent.h"
#include "DrawDebugHelpers.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	utilityAI = CreateDefaultSubobject<UUtilityAIComponent>(TEXT("UtilityAI"));
}

void AEnemyBase::InitTurn()
{
	Super::InitTurn();

	if (AEnemyAIController* aic = Cast<AEnemyAIController>(GetController()))
	{
		aic->OnEnemyTurnStart();
	}
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bShowDetectionDebug) return;

	// 부채꼴 시각화 — 두 가장자리 선 + 중심선 + 반경 원호 샘플링
	const FVector origin = GetActorLocation();
	const FVector forward = GetActorForwardVector();
	const float halfAngle = detectionAngle * 0.5f;
	const FRotator yawL(0.f, -halfAngle, 0.f);
	const FRotator yawR(0.f, +halfAngle, 0.f);

	DrawDebugLine(GetWorld(), origin, origin + forward * detectionRadius, FColor::Yellow, false, -1.f, 0, 1.f);
	DrawDebugLine(GetWorld(), origin, origin + yawL.RotateVector(forward) * detectionRadius, FColor::Red, false, -1.f, 0, 1.f);
	DrawDebugLine(GetWorld(), origin, origin + yawR.RotateVector(forward) * detectionRadius, FColor::Red, false, -1.f, 0, 1.f);
}

bool AEnemyBase::IsInDetectionFan(const FVector& worldPoint) const
{
	// 시야 차단(라인 트레이스)은 여기서 수행하지 않음 — AIController 책임
	const FVector toTarget = worldPoint - GetActorLocation();
	const float distSq = toTarget.SizeSquared();

	if (distSq <= KINDA_SMALL_NUMBER) return true;
	if (distSq > detectionRadius * detectionRadius) return false;

	// 전방위(360) 케이스 — 각도 검사 스킵
	if (detectionAngle >= 360.f) return true;

	const FVector forward = GetActorForwardVector();
	const FVector dir = toTarget * FMath::InvSqrt(distSq);

	const float cosHalfAngle = FMath::Cos(FMath::DegreesToRadians(detectionAngle * 0.5f));
	return FVector::DotProduct(forward, dir) >= cosHalfAngle;
}

void AEnemyBase::HandleDeath()
{
	// 기반 클래스의 lastAttacker를 Ally로 캐스팅 — 경험치 분배는 아군 처치 한정
	AAllyCharacterBase* killer = Cast<AAllyCharacterBase>(lastAttacker.Get());

	UE_LOG(LogTemp, Log, TEXT("[EnemyBase] %s 사망 — 처치자: %s"),
		*GetName(),
		killer ? *killer->GetName() : TEXT("없음"));

	// 경험치 분배 (Destroy 전에 호출해야 유효한 참조 전달)
	OnEnemyDeath.Broadcast(this, killer);

	Super::HandleDeath();
}
