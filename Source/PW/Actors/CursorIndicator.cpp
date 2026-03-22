// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/CursorIndicator.h"
#include "Characters/CharacterBase.h"
#include "Widget/MoveIndicatorWidget.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SplineComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"

ACursorIndicator::ACursorIndicator()
{
	PrimaryActorTick.bCanEverTick = true;

	// 루트 컴포넌트
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// 경로 스플라인 (자체 드로우 비활성 — 직접 DrawDebugLine으로 렌더링)
	pathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	pathSpline->SetupAttachment(SceneRoot);
	pathSpline->SetDrawDebug(false);

	// 바닥 데칼
	moveDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("MoveDecal"));
	moveDecal->SetupAttachment(SceneRoot);
	// 데칼은 아래를 향하도록 회전 (DecalComponent 기본 방향: -X)
	moveDecal->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));

	// 거리 표시 위젯 컴포넌트 (Widget Class는 BP_CursorIndicator에서 WBP_MoveIndicator로 지정)
	distanceWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DistanceWidget"));
	distanceWidget->SetupAttachment(SceneRoot);
	distanceWidget->SetDrawSize(FVector2D(300, 100));
	distanceWidget->SetWidgetSpace(EWidgetSpace::Screen);
}

void ACursorIndicator::BeginPlay()
{
	Super::BeginPlay();

	distanceWidget->SetRelativeLocation(widgetOffset);
}

void ACursorIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ─── 0. 이동 완료 감지 → 잠금 자동 해제 ─────────────────
	if (bIsLocked)
	{
		if (!IsValid(activeUnit) || !activeUnit->IsMoving())
		{
			bIsLocked = false;
		}
		else
		{
			// 이동 중: 데칼·위젯을 고정 위치에 유지하고 나머지 로직 건너뜀
			moveDecal->SetWorldLocation(lockedIndicatorPos);
			distanceWidget->SetWorldLocation(lockedIndicatorPos + widgetOffset);
			return;
		}
	}

	// ─── 1. 커서 위치로 액터 이동 ────────────────────────────
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	FHitResult HitResult;
	if (PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		SetActorLocation(HitResult.Location);
	}

	// ─── 2. 경로 거리 갱신 (0.1초 쓰로틀링) ─────────────────
	pathUpdateTimer += DeltaTime;
	if (pathUpdateTimer >= pathUpdateInterval)
	{
		pathUpdateTimer = 0.f;
		UpdatePathDistance();
	}

	// ─── 3. 경로 시각화 (흰색: 이동 가능, 빨간색: 이동력 초과) ──
	const int32 numPoints = cachedPathPoints.Num();
	for (int32 i = 0; i + 1 < numPoints; ++i)
	{
		const FVector A = cachedPathPoints[i]     + FVector(0, 0, 5.f);
		const FVector B = cachedPathPoints[i + 1] + FVector(0, 0, 5.f);

		if (cachedSplitSegIndex == -1 || i < cachedSplitSegIndex)
		{
			// 이동 가능 범위 전체 세그먼트 → 흰색
			DrawDebugLine(GetWorld(), A, B, FColor::White, false, -1.f, 0, 3.f);
		}
		else if (i == cachedSplitSegIndex)
		{
			// 분기점을 포함하는 세그먼트 → 앞부분 흰색 / 뒷부분 빨간색
			const FVector SplitZ = cachedSplitPoint + FVector(0, 0, 5.f);
			DrawDebugLine(GetWorld(), A,      SplitZ, FColor::White, false, -1.f, 0, 3.f);
			DrawDebugLine(GetWorld(), SplitZ, B,      FColor::Red,   false, -1.f, 0, 3.f);
		}
		else
		{
			// 이동력 초과 범위 → 빨간색
			DrawDebugLine(GetWorld(), A, B, FColor::Red, false, -1.f, 0, 3.f);
		}
	}

	// ─── 4. 데칼·위젯 위치: 이동 가능 범위 경계까지만 따라오기 ──
	// 액터 자체는 커서 위치를 유지 (NavMesh 경로 도착점으로 사용됨)
	// 컴포넌트만 독립적으로 위치 조정
	const FVector indicatorPos = (cachedSplitSegIndex == -1)
		? GetActorLocation()      // 이동 가능 범위 내 → 커서 위치
		: cachedSplitPoint;       // 이동력 초과 → 분기점에 고정

	moveDecal->SetWorldLocation(indicatorPos);
	distanceWidget->SetWorldLocation(indicatorPos + widgetOffset);
}

void ACursorIndicator::SetActiveUnit(ACharacterBase* Unit)
{
	activeUnit = Unit;
}

void ACursorIndicator::LockAtCurrentPosition()
{
	// 이동 명령 시점의 데칼 위치(이동 가능 범위 경계)를 고정 위치로 저장
	lockedIndicatorPos = (cachedSplitSegIndex == -1)
		? GetActorLocation()
		: cachedSplitPoint;
	bIsLocked = true;
}

void ACursorIndicator::UpdatePathDistance()
{
	if (!IsValid(activeUnit)) return;

	UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(
		GetWorld(),
		activeUnit->GetActorLocation(),
		GetActorLocation()
	);

	if (Path && Path->IsValid())
	{
		// 경로 경유점 캐싱 (이동 명령 시 재사용)
		cachedPathPoints.Empty();
		for (const FNavPathPoint& Point : Path->GetPath()->GetPathPoints())
		{
			cachedPathPoints.Add(Point.Location);
		}

		// 스플라인 포인트 갱신
		pathSpline->ClearSplinePoints(false);
		for (const FVector& Point : cachedPathPoints)
		{
			pathSpline->AddSplinePoint(Point, ESplineCoordinateSpace::World, false);
		}
		pathSpline->UpdateSpline();

		// 이동력 분기점 계산
		UpdateSplitPoint();

		const float Meters = Path->GetPathLength() / 100.f;

		if (UMoveIndicatorWidget* Widget = Cast<UMoveIndicatorWidget>(distanceWidget->GetWidget()))
		{
			Widget->UpdateDistance(Meters);
		}
	}
	else
	{
		cachedPathPoints.Empty();
		pathSpline->ClearSplinePoints();
		cachedSplitSegIndex = -1;
	}
}

void ACursorIndicator::UpdateSplitPoint()
{
	cachedSplitSegIndex = -1;

	if (!IsValid(activeUnit) || cachedPathPoints.Num() < 2) return;

	// 현재 이동력(m)을 cm으로 변환
	const float budgetCm = activeUnit->GetCurrentMovingPoint() * 100.f;
	float accumulated = 0.f;

	for (int32 i = 0; i + 1 < cachedPathPoints.Num(); ++i)
	{
		const float segLen = FVector::Dist(cachedPathPoints[i], cachedPathPoints[i + 1]);

		if (accumulated + segLen >= budgetCm)
		{
			// 이 세그먼트 안에서 이동력 소진 → Lerp로 정확한 분기점 보간
			const float t = (budgetCm - accumulated) / segLen;
			cachedSplitPoint = FMath::Lerp(cachedPathPoints[i], cachedPathPoints[i + 1], t);
			cachedSplitSegIndex = i;
			return;
		}
		accumulated += segLen;
	}
	// 누적 거리가 예산 미만 → 경로 전체가 이동 가능 (cachedSplitSegIndex = -1 유지)
}
