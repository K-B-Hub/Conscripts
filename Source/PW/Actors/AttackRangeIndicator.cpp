// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/AttackRangeIndicator.h"
#include "Actors/CursorIndicator.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "Characters/EnemyBase.h"
#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

AAttackRangeIndicator::AAttackRangeIndicator()
{
	PrimaryActorTick.bCanEverTick = true;

	rootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = rootScene;

	overlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	overlapSphere->SetupAttachment(RootComponent);
	overlapSphere->SetSphereRadius(1.f);
	overlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	overlapSphere->SetCollisionObjectType(ECC_WorldDynamic);
	overlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	overlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	overlapSphere->SetGenerateOverlapEvents(true);

	areaDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("AreaDecal"));
	areaDecal->SetupAttachment(RootComponent);
	areaDecal->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	areaDecal->SetVisibility(false);
}

void AAttackRangeIndicator::BeginPlay()
{
	Super::BeginPlay();

	overlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AAttackRangeIndicator::OnOverlapBegin);
	overlapSphere->OnComponentEndOverlap.AddDynamic(this, &AAttackRangeIndicator::OnOverlapEnd);
}

void AAttackRangeIndicator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroyMovePathIndicator();
	Super::EndPlay(EndPlayReason);
}

void AAttackRangeIndicator::InitIndicator(ACharacterBase* InCaster, UActiveSkillBase* Skill)
{
	caster = InCaster;
	cachedSkill = Skill;
	pickRange = Skill->pickRange;
	bIsAreaAttack = (Skill->areaTarget != EAreaTarget::None);
	bFixedAtCaster = (Skill->selectMode == ESelectMode::Self);

	if (bIsAreaAttack)
	{
		if (areaDecalMaterial)
		{
			areaDecal->SetDecalMaterial(areaDecalMaterial);
		}
		areaDecal->SetVisibility(true);

		float OverlapRadius = 0.f;
		switch (Skill->areaForm)
		{
		case EAreaForm::Circle:
			OverlapRadius = Skill->areaParameter1;
			areaDecal->DecalSize = FVector(decalProjectionDepth, Skill->areaParameter1, Skill->areaParameter1);
			break;
		case EAreaForm::Cone:
			OverlapRadius = Skill->areaParameter1;
			areaDecal->DecalSize = FVector(decalProjectionDepth, Skill->areaParameter1, Skill->areaParameter1);
			break;
		case EAreaForm::Ray:
			OverlapRadius = FMath::Max(Skill->areaParameter1, Skill->areaParameter2) * 0.5f;
			areaDecal->DecalSize = FVector(decalProjectionDepth, Skill->areaParameter1 * 0.5f, Skill->areaParameter2 * 0.5f);
			break;
		}
		overlapSphere->SetSphereRadius(FMath::Max(OverlapRadius, 1.f));
	}
	else
	{
		areaDecal->SetVisibility(false);
		overlapSphere->SetSphereRadius(10.f);
	}
}

void AAttackRangeIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bFixedAtCaster)
	{
		if (caster.IsValid())
		{
			SetActorLocation(caster->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
		}
		DrawDebugSphere(GetWorld(), overlapSphere->GetComponentLocation(),
			overlapSphere->GetScaledSphereRadius(), 24, FColor::Cyan, false, 0.f);
		return;
	}

	if (bIsLocked)
	{
		DrawDebugSphere(GetWorld(), overlapSphere->GetComponentLocation(),
			overlapSphere->GetScaledSphereRadius(), 24, FColor::Yellow, false, 0.f);
		return;
	}

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	FHitResult HitResult;
	FVector TargetLocation;
	ACharacterBase* NewSnappedTarget = nullptr;

	if (cachedSkill && cachedSkill->selectMode == ESelectMode::SinglePick)
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> PawnObjectTypes;
		PawnObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		FHitResult PawnHit;
		if (PC->GetHitResultUnderCursorForObjects(PawnObjectTypes, false, PawnHit))
		{
			ACharacterBase* HitCharacter = Cast<ACharacterBase>(PawnHit.GetActor());
			if (HitCharacter && HitCharacter != caster.Get() && IsPickTarget(HitCharacter))
			{
				TargetLocation = HitCharacter->GetActorLocation();
				NewSnappedTarget = HitCharacter;
			}
		}
	}

	if (!NewSnappedTarget)
	{
		if (!PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult)) return;
		TargetLocation = HitResult.Location;
	}

	snappedTarget = NewSnappedTarget;

	bIsOutOfRange = false;
	if (pickRange > 0.f && caster.IsValid())
	{
		FVector Offset = TargetLocation - caster->GetActorLocation();
		Offset.Z = 0.f;
		if (Offset.Size() > pickRange)
		{
			bIsOutOfRange = true;
		}
	}

	SetActorLocation(TargetLocation, false, nullptr, ETeleportType::TeleportPhysics);

	const bool bIsMultiPick = cachedSkill && cachedSkill->pickCount > 1;
	if (bIsOutOfRange && !bIsMultiPick)
	{
		// 쓰로틀링된 최적 이동 지점 계산
		optimalPointUpdateTimer += DeltaTime;
		if (optimalPointUpdateTimer >= optimalPointUpdateInterval)
		{
			optimalPointUpdateTimer = 0.f;
			ComputeOptimalMovePoint();
		}

		if (!movePathIndicator) SpawnMovePathIndicator();
		if (movePathIndicator) movePathIndicator->SetTargetOverride(moveToPoint);
	}
	else
	{
		optimalPointUpdateTimer = 0.f;
		DestroyMovePathIndicator();
	}

	const FColor DebugColor = bIsOutOfRange ? FColor::Red : FColor::Green;
	DrawDebugSphere(GetWorld(), overlapSphere->GetComponentLocation(),
		overlapSphere->GetScaledSphereRadius(), 24, DebugColor, false, 0.f);
}

bool AAttackRangeIndicator::ComputeOutOfRange() const
{
	if (pickRange <= 0.f || !caster.IsValid()) return false;
	FVector Offset = GetActorLocation() - caster->GetActorLocation();
	Offset.Z = 0.f;
	return Offset.SizeSquared() > pickRange * pickRange;
}

const TArray<FVector>& AAttackRangeIndicator::GetMovePath() const
{
	if (movePathIndicator)
	{
		return movePathIndicator->GetCachedPathPoints();
	}
	static TArray<FVector> Empty;
	return Empty;
}

void AAttackRangeIndicator::LockAtCurrentPosition()
{
	bIsLocked = true;
	if (movePathIndicator)
	{
		movePathIndicator->LockAtCurrentPosition();
	}
}

void AAttackRangeIndicator::Unlock()
{
	bIsLocked = false;
	DestroyMovePathIndicator();
}

void AAttackRangeIndicator::ComputeOptimalMovePoint()
{
	if (!caster.IsValid() || !cachedSkill) return;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return;

	const FVector CasterLocation = caster->GetActorLocation();
	const FVector TargetLocation = GetActorLocation();

	// 시전자 → 인디케이터 위치까지 NavMesh 경로 계산
	UNavigationPath* Path = NavSys->FindPathToLocationSynchronously(
		GetWorld(), CasterLocation, TargetLocation);
	if (!Path || !Path->IsValid()) return;

	const TArray<FNavPathPoint>& PathPoints = Path->GetPath()->GetPathPoints();
	if (PathPoints.Num() < 2) return;

	// 경로를 시전자→타겟 방향으로 순회하며
	// 거리 < 사거리 && 시야선 확보인 첫 지점을 자동이동 지점으로 선택
	// 경로 포인트 사이를 보간해 사거리 경계 지점을 정확히 계산
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(caster.Get());
	const FVector EyeHeight(0.f, 0.f, 80.f);

	auto Dist2DToTarget = [&](const FVector& Point) -> float
	{
		FVector Offset = TargetLocation - Point;
		Offset.Z = 0.f;
		return Offset.Size();
	};

	for (int32 i = 0; i + 1 < PathPoints.Num(); ++i)
	{
		const FVector& A = PathPoints[i].Location;
		const FVector& B = PathPoints[i + 1].Location;
		const float DistA = Dist2DToTarget(A);
		const float DistB = Dist2DToTarget(B);

		// 이 세그먼트에서 사거리 안으로 진입하는 경우 → 경계 지점 보간
		FVector Candidate;
		if (DistA >= pickRange && DistB < pickRange)
		{
			// A(밖)→B(안) 구간에서 정확한 경계 지점 보간
			const float t = (DistA - pickRange) / (DistA - DistB);
			Candidate = FMath::Lerp(A, B, t);
		}
		else if (DistA < pickRange)
		{
			// 이미 사거리 안 — 이 지점 사용
			Candidate = A;
		}
		else
		{
			continue;
		}

		// 시야선 체크
		FHitResult LOSHit;
		bool bBlocked = GetWorld()->LineTraceSingleByChannel(
			LOSHit,
			Candidate + EyeHeight,
			TargetLocation + EyeHeight,
			ECC_Visibility,
			TraceParams);

		if (!bBlocked)
		{
			moveToPoint = Candidate;
			return;
		}
	}

	// 마지막 포인트(타겟 위치) 체크
	const FVector& LastPoint = PathPoints.Last().Location;
	if (Dist2DToTarget(LastPoint) < pickRange)
	{
		FHitResult LOSHit;
		bool bBlocked = GetWorld()->LineTraceSingleByChannel(
			LOSHit,
			LastPoint + EyeHeight,
			TargetLocation + EyeHeight,
			ECC_Visibility,
			TraceParams);

		if (!bBlocked)
		{
			moveToPoint = LastPoint;
			return;
		}
	}
}

void AAttackRangeIndicator::SpawnMovePathIndicator()
{
	if (movePathIndicator || !cursorIndicatorClass || !caster.IsValid()) return;

	AAllyCharacterBase* AllyCaster = Cast<AAllyCharacterBase>(caster.Get());
	if (!AllyCaster) return;

	movePathIndicator = GetWorld()->SpawnActor<ACursorIndicator>(cursorIndicatorClass);
	if (movePathIndicator)
	{
		movePathIndicator->SetActiveUnit(AllyCaster);
	}
}

void AAttackRangeIndicator::DestroyMovePathIndicator()
{
	if (IsValid(movePathIndicator))
	{
		movePathIndicator->Destroy();
		movePathIndicator = nullptr;
	}
}

bool AAttackRangeIndicator::IsAreaTarget(ACharacterBase* Character) const
{
	if (!cachedSkill) return false;

	switch (cachedSkill->areaTarget)
	{
	case EAreaTarget::EnemyOnly:
		return Cast<AEnemyBase>(Character) != nullptr;
	case EAreaTarget::AllyOnly:
		return Cast<AAllyCharacterBase>(Character) != nullptr;
	case EAreaTarget::All:
		return true;
	case EAreaTarget::None:
		return true;
	}
	return false;
}

bool AAttackRangeIndicator::IsPickTarget(ACharacterBase* Character) const
{
	if (!cachedSkill || !Character) return false;

	switch (cachedSkill->pickTeam)
	{
	case EPickTeam::EnemyOnly:
		return Cast<AEnemyBase>(Character) != nullptr;
	case EPickTeam::AllyOnly:
		return Cast<AAllyCharacterBase>(Character) != nullptr;
	case EPickTeam::Any:
		return true;
	}
	return false;
}

void AAttackRangeIndicator::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacterBase* Character = Cast<ACharacterBase>(OtherActor);
	if (!Character) return;

	if (caster.IsValid() && Character == caster.Get()) return;

	//SkillType에 맞는 대상이 아닐시 return
	if (!IsAreaTarget(Character)) return;

	overlappingTargets.AddUnique(Character);

	if (caster.IsValid())
	{
		if (USkillComponent* SkillComp = caster->FindComponentByClass<USkillComponent>())
		{
			SkillComp->AddTarget(Character);
		}
	}

	if (cachedSkill)
	{
		Character->CalculateDamage(
			cachedSkill->calcDamage,
			cachedSkill->calcAccuracy,
			cachedSkill->calcCritical,
			cachedSkill->calcDamageAmplfication,
			cachedSkill->calcPenetration
		);
		Character->ShowSkillInfo();
	}
}

void AAttackRangeIndicator::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacterBase* Character = Cast<ACharacterBase>(OtherActor);
	if (!Character) return;

	overlappingTargets.Remove(Character);

	if (caster.IsValid())
	{
		if (USkillComponent* SkillComp = caster->FindComponentByClass<USkillComponent>())
		{
			SkillComp->RemoveTarget(Character);
		}
	}

	Character->HideSkillInfo();
	Character->ClearPendingDamage();
}
