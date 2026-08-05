// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/AttackRangeIndicator.h"
#include "Actors/CursorIndicator.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Widget/MoveIndicatorWidget.h"
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

	//아치 궤적 메쉬는 월드 좌표를 직접 쓰도록 절대 좌표 루트에 부착
	arcMeshRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ArcMeshRoot"));
	arcMeshRoot->SetupAttachment(RootComponent);
	arcMeshRoot->SetAbsolute(true, true, true);

	arcCostWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ArcCostWidget"));
	arcCostWidget->SetupAttachment(RootComponent);
	arcCostWidget->SetDrawSize(FVector2D(300, 100));
	arcCostWidget->SetWidgetSpace(EWidgetSpace::Screen);
	arcCostWidget->SetVisibility(false);
}

void AAttackRangeIndicator::BeginPlay()
{
	Super::BeginPlay();

	overlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AAttackRangeIndicator::OnOverlapBegin);
	overlapSphere->OnComponentEndOverlap.AddDynamic(this, &AAttackRangeIndicator::OnOverlapEnd);
}

void AAttackRangeIndicator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//Self 스킬은 overlap 종료 경로를 타지 않으므로 caster 정보 위젯을 수동 정리
	if (bFixedAtCaster && caster.IsValid())
	{
		caster->HideSkillInfo();
		caster->ClearPendingDamage();
	}
	DestroyMovePathIndicator();
	Super::EndPlay(EndPlayReason);
}

void AAttackRangeIndicator::InitIndicator(ACharacterBase* InCaster, UActiveSkillBase* Skill)
{
	caster = InCaster;
	cachedSkill = Skill;
	//점프는 현재 이동력으로 갈 수 있는 거리로 사거리 제한, 그 외는 pickRange 그대로
	pickRange = Skill->GetEffectivePickRange();
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

	//Self 스킬은 커서·overlap 없이 caster 자신이 대상, 즉시 타겟 등록 및 예측·정보 위젯 표시
	if (bFixedAtCaster && caster.IsValid())
	{
		if (USkillComponent* SkillComp = caster->GetSkillComponent())
		{
			SkillComp->AddTarget(caster.Get());
		}
		RecalculatePendingForTarget(caster.Get());
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
			//사거리 내지만 시야선 차단 시에도 자동이동 필요, caster·target 자세 눈높이 적용
			FCollisionQueryParams LOSParams;
			LOSParams.AddIgnoredActor(caster.Get());
			const FVector CasterEye(0.f, 0.f, caster->GetEyeHeightZ());
			const FVector TargetEye(0.f, 0.f, NewSnappedTarget ? NewSnappedTarget->GetEyeHeightZ() : ACharacterBase::StandingEyeHeightZ);
			FHitResult LOSHit;
			bNeedsAutoMove = GetWorld()->LineTraceSingleByChannel(
				LOSHit,
				CasterLoc + CasterEye,
				TargetLocation + TargetEye,
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

	//아치 스킬(점프·투척)은 자동이동 경로 인디케이터를 쓰지 않음
	const bool bArc = cachedSkill && cachedSkill->arcApexRatio > 0.f;
	const bool bIsMultiPick = cachedSkill && cachedSkill->pickCount > 1;
	if (bNeedsAutoMove && !bIsMultiPick && !bArc)
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

	//아치 스킬은 자체 궤적 메쉬로 표시하므로 디버그 구체 생략
	if (!bArc)
	{
		const FColor DebugColor = bNeedsAutoMove ? FColor::Red : FColor::Green;
		DrawDebugSphere(GetWorld(), overlapSphere->GetComponentLocation(),
			overlapSphere->GetScaledSphereRadius(), 24, DebugColor, false, 0.f);
	}

	//아치 스킬(점프·투척): 시전자→커서 지점 포물선 궤적을 메쉬로, 착지 데칼·비용 위젯 표시
	if (bArc && caster.IsValid())
	{
		const FVector ArcFrom = caster->GetActorLocation();
		//끝점은 지면 위치이므로 캡슐 half-height만큼 올려 착지 시 캡슐 중심 높이를 맞춤
		FVector ArcTo = GetActorLocation();
		ArcTo.Z += caster->GetSimpleCollisionHalfHeight();
		const float HorizDist = FVector2D(ArcTo.X - ArcFrom.X, ArcTo.Y - ArcFrom.Y).Size();
		arcPath = BuildArcPath(ArcFrom, ArcTo, HorizDist * cachedSkill->arcApexRatio, arcSampleCount);

		//도달 가능 여부: pickRange가 점프의 이동력 예산을 이미 반영(GetEffectivePickRange)
		const bool bReachable = HorizDist <= pickRange;

		//궤적 메쉬(도달=흰/불가=빨강 재질)
		RebuildArcMeshes(bReachable);

		//착지 데칼 표시, 기존 areaDecal(BP 지정 재질) 그대로 재사용
		areaDecal->DecalSize = FVector(decalProjectionDepth, cachedSkill->areaParameter1, cachedSkill->areaParameter1);
		areaDecal->SetVisibility(true);

		//이동력 비용 위젯: 투척이 아닌 아치 스킬(점프)만 표시
		if (!cachedSkill->IsThrow())
		{
			const float CostM = (HorizDist / 100.f) * cachedSkill->arcMoveCostMultiplier;
			arcCostWidget->SetVisibility(true);
			arcCostWidget->SetWorldLocation(GetActorLocation() + arcWidgetOffset);
			if (UMoveIndicatorWidget* W = Cast<UMoveIndicatorWidget>(arcCostWidget->GetWidget()))
			{
				W->UpdateDistance(CostM);
			}
		}
		else
		{
			arcCostWidget->SetVisibility(false);
		}
	}
}

TArray<FVector> AAttackRangeIndicator::BuildArcPath(const FVector& From, const FVector& To, float ApexHeight, int32 Samples)
{
	if (Samples < 2) Samples = 2;

	TArray<FVector> Points;
	Points.Reserve(Samples + 1);

	//수평은 선형 보간, 수직은 정점이 중간에 오는 포물선 z += apex*4t(1-t)
	for (int32 i = 0; i <= Samples; ++i)
	{
		const float t = static_cast<float>(i) / Samples;
		FVector p = FMath::Lerp(From, To, t);
		p.Z += ApexHeight * 4.f * t * (1.f - t);
		Points.Add(p);
	}
	return Points;
}

void AAttackRangeIndicator::RebuildArcMeshes(bool bReachable)
{
	//메쉬 에셋 없거나 궤적 없음 → 풀 비우고 종료
	if (!arcSegmentMesh || arcPath.Num() < 2)
	{
		for (USplineMeshComponent* Mesh : arcMeshes)
		{
			if (IsValid(Mesh)) Mesh->DestroyComponent();
		}
		arcMeshes.Empty();
		return;
	}

	const int32 neededSegments = arcPath.Num() - 1;

	//세그먼트 수가 바뀔 때만 컴포넌트 생성/파괴, 평소엔 트랜스폼만 갱신
	while (arcMeshes.Num() < neededSegments)
	{
		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
		SplineMesh->SetStaticMesh(arcSegmentMesh);
		SplineMesh->SetMobility(EComponentMobility::Movable);
		SplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SplineMesh->SetupAttachment(arcMeshRoot);
		SplineMesh->RegisterComponent();
		arcMeshes.Add(SplineMesh);
	}
	while (arcMeshes.Num() > neededSegments)
	{
		if (USplineMeshComponent* Mesh = arcMeshes.Pop())
		{
			if (IsValid(Mesh)) Mesh->DestroyComponent();
		}
	}

	//Catmull-Rom 접선 계산, 이전/다음 점 방향의 평균
	auto GetTangent = [&](int32 idx) -> FVector
	{
		const int32 last = arcPath.Num() - 1;
		if (idx <= 0)    return arcPath[1] - arcPath[0];
		if (idx >= last) return arcPath[last] - arcPath[last - 1];
		return (arcPath[idx + 1] - arcPath[idx - 1]) * 0.5f;
	};

	UMaterialInterface* Mat = bReachable ? arcMaterialReachable : arcMaterialUnreachable;

	//풀링된 세그먼트마다 트랜스폼·재질만 갱신
	for (int32 i = 0; i < neededSegments; ++i)
	{
		USplineMeshComponent* SplineMesh = arcMeshes[i];
		if (!IsValid(SplineMesh)) continue;

		SplineMesh->SetStartAndEnd(arcPath[i], GetTangent(i), arcPath[i + 1], GetTangent(i + 1), true);
		SplineMesh->SetStartScale(arcMeshScale, false);
		SplineMesh->SetEndScale(arcMeshScale, true);

		if (Mat) SplineMesh->SetMaterial(0, Mat);
	}
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

	//시야선 체크, caster·target 자세 눈높이 적용
	FCollisionQueryParams LOSParams;
	LOSParams.AddIgnoredActor(caster.Get());
	const FVector CasterEye(0.f, 0.f, caster->GetEyeHeightZ());
	const FVector TargetEye(0.f, 0.f, snappedTarget.IsValid() ? snappedTarget->GetEyeHeightZ() : ACharacterBase::StandingEyeHeightZ);
	FHitResult LOSHit;
	return GetWorld()->LineTraceSingleByChannel(
		LOSHit,
		CasterLoc + CasterEye,
		TargetLoc + TargetEye,
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
	//후보 지점은 시전자(이동 후) 눈높이, 타겟은 대상 자세 눈높이 적용
	const FVector CasterEye(0.f, 0.f, caster.IsValid() ? caster->GetEyeHeightZ() : ACharacterBase::StandingEyeHeightZ);
	const FVector TargetEye(0.f, 0.f, snappedTarget.IsValid() ? snappedTarget->GetEyeHeightZ() : ACharacterBase::StandingEyeHeightZ);

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
			Candidate + CasterEye,
			TargetLocation + TargetEye,
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
			LastPoint + CasterEye,
			TargetLocation + TargetEye,
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
