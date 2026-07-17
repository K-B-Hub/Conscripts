// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/AttackRangeIndicator.h"
#include "Actors/CursorIndicator.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "Characters/EnemyBase.h"
#include "ActorComponent/SkillComponent.h"
#include "ActorComponent/PassiveSkillComponent.h"
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
	bool bNeedsAutoMove = false;
	if (pickRange > 0.f && caster.IsValid())
	{
		const FVector CasterLoc = caster->GetActorLocation();
		FVector Offset = TargetLocation - CasterLoc;
		Offset.Z = 0.f;
		if (Offset.Size() > pickRange)
		{
			bIsOutOfRange = true;
			bNeedsAutoMove = true;
		}
		else
		{
			//사거리 내지만 시야선 차단 시에도 자동이동 필요
			FCollisionQueryParams LOSParams;
			LOSParams.AddIgnoredActor(caster.Get());
			const FVector EyeHeight(0.f, 0.f, 80.f);
			FHitResult LOSHit;
			bNeedsAutoMove = GetWorld()->LineTraceSingleByChannel(
				LOSHit,
				CasterLoc + EyeHeight,
				TargetLocation + EyeHeight,
				ECC_Visibility,
				LOSParams);
		}
	}

	SetActorLocation(TargetLocation, false, nullptr, ETeleportType::TeleportPhysics);

	//자동이동 필요 여부 전이 감지
	if (bNeedsAutoMove != bPrevAutoMoveNeeded)
	{
		if (caster.IsValid())
		{
			caster->OnMoveStateChanged(bNeedsAutoMove);
			//이동 상태 변경으로 데미지 계산이 달라지므로 오버랩 중인 타겟도 재계산
			for (ACharacterBase* OverlapTarget : overlappingTargets)
			{
				if (IsValid(OverlapTarget))
				{
					RecalculatePendingForTarget(OverlapTarget);
				}
			}
		}
		bPrevAutoMoveNeeded = bNeedsAutoMove;
	}

	const bool bIsMultiPick = cachedSkill && cachedSkill->pickCount > 1;
	if (bNeedsAutoMove && !bIsMultiPick)
	{
		//쓰로틀링된 최적 이동 지점 계산
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

	const FColor DebugColor = bNeedsAutoMove ? FColor::Red : FColor::Green;
	DrawDebugSphere(GetWorld(), overlapSphere->GetComponentLocation(),
		overlapSphere->GetScaledSphereRadius(), 24, DebugColor, false, 0.f);
}

bool AAttackRangeIndicator::ComputeOutOfRange() const
{
	if (pickRange <= 0.f || !caster.IsValid()) return false;

	const FVector CasterLoc = caster->GetActorLocation();
	const FVector TargetLoc = GetActorLocation();

	//사거리 체크
	FVector Offset = TargetLoc - CasterLoc;
	Offset.Z = 0.f;
	if (Offset.SizeSquared() > pickRange * pickRange) return true;

	//시야선 체크
	FCollisionQueryParams LOSParams;
	LOSParams.AddIgnoredActor(caster.Get());
	const FVector EyeHeight(0.f, 0.f, 80.f);
	FHitResult LOSHit;
	return GetWorld()->LineTraceSingleByChannel(
		LOSHit,
		CasterLoc + EyeHeight,
		TargetLoc + EyeHeight,
		ECC_Visibility,
		LOSParams);
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

	//시전자 → 인디케이터 위치까지 NavMesh 경로 계산
	UNavigationPath* Path = NavSys->FindPathToLocationSynchronously(
		GetWorld(), CasterLocation, TargetLocation);
	if (!Path || !Path->IsValid()) return;

	const TArray<FNavPathPoint>& PathPoints = Path->GetPath()->GetPathPoints();
	if (PathPoints.Num() < 2) return;

	//경로를 순회하며 사거리 내 + 시야선 확보된 첫 지점을 자동이동 지점으로 선택
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

		//사거리 안으로 진입하는 세그먼트면 경계 지점 보간
		FVector Candidate;
		if (DistA >= pickRange && DistB < pickRange)
		{
			const float t = (DistA - pickRange) / (DistA - DistB);
			Candidate = FMath::Lerp(A, B, t);
		}
		else if (DistA < pickRange)
		{
			//이미 사거리 안이면 이 지점 사용
			Candidate = A;
		}
		else
		{
			continue;
		}

		//시야선 체크
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

	//마지막 포인트(타겟 위치) 체크
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
	if (!cachedSkill || !Character || !caster.IsValid()) return false;

	//시야 밖 적은 범위 대상에서 제외
	if (const AEnemyBase* enemy = Cast<AEnemyBase>(Character))
	{
		if (!enemy->IsVisibleToPlayers()) return false;
	}

	//caster 팀 기준 판단
	const bool bSameTeam = (Cast<AAllyCharacterBase>(caster.Get()) != nullptr)
		== (Cast<AAllyCharacterBase>(Character) != nullptr);

	switch (cachedSkill->areaTarget)
	{
	case EAreaTarget::EnemyOnly:
		return !bSameTeam;
	case EAreaTarget::AllyOnly:
		return bSameTeam;
	case EAreaTarget::All:
		return true;
	case EAreaTarget::None:
		return true;
	}
	return false;
}

bool AAttackRangeIndicator::IsPickTarget(ACharacterBase* Character) const
{
	if (!cachedSkill || !Character || !caster.IsValid()) return false;

	//시야 밖 적은 조준 불가
	if (const AEnemyBase* enemy = Cast<AEnemyBase>(Character))
	{
		if (!enemy->IsVisibleToPlayers()) return false;
	}

	//caster 팀 기준 판단
	const bool bSameTeam = (Cast<AAllyCharacterBase>(caster.Get()) != nullptr)
		== (Cast<AAllyCharacterBase>(Character) != nullptr);

	switch (cachedSkill->pickTeam)
	{
	case EPickTeam::EnemyOnly:
		return !bSameTeam;
	case EPickTeam::AllyOnly:
		return bSameTeam;
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

	//스킬 모드에 맞는 필터 적용, 비범위 스킬은 pickTeam 기준
	if (bIsAreaAttack ? !IsAreaTarget(Character) : !IsPickTarget(Character)) return;

	overlappingTargets.AddUnique(Character);

	if (caster.IsValid())
	{
		if (USkillComponent* SkillComp = caster->FindComponentByClass<USkillComponent>())
		{
			SkillComp->AddTarget(Character);
		}
	}

	RecalculatePendingForTarget(Character);
}

void AAttackRangeIndicator::RecalculatePendingForTarget(ACharacterBase* Target)
{
	if (!Target || !caster.IsValid()) return;

	//데미지 계산은 SkillComponent에 위임하고 UI 표시만 추가
	if (USkillComponent* SkillComp = caster->GetSkillComponent())
	{
		SkillComp->RecalculatePending(Target);
	}
	Target->ShowSkillInfo();
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
