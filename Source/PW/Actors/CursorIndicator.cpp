// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/CursorIndicator.h"
#include "Characters/AllyCharacterBase.h"
#include "Widget/MoveIndicatorWidget.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

ACursorIndicator::ACursorIndicator()
{
	PrimaryActorTick.bCanEverTick = true;

	// 루트 컴포넌트
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// 경로 스플라인 (접선 계산 전용 — SplineMeshComponent가 시각화 담당)
	pathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	pathSpline->SetupAttachment(SceneRoot);
	pathSpline->SetDrawDebug(false);

	// 경로 메쉬 루트: 절대 월드 좌표 고정
	// 액터가 커서를 따라 이동해도 경로 메쉬 위치가 흔들리지 않도록 부모 트랜스폼 차단
	pathMeshRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PathMeshRoot"));
	pathMeshRoot->SetupAttachment(SceneRoot);
	pathMeshRoot->SetAbsolute(true, true, true);

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

	// ─── 3. 데칼·위젯 위치: 이동 가능 범위 경계까지만 따라오기 ──
	// (경로 시각화는 UpdatePathDistance에서 RebuildPathMeshes로 처리)
	// 액터 자체는 커서 위치를 유지 (NavMesh 경로 도착점으로 사용됨)
	// 컴포넌트만 독립적으로 위치 조정
	const FVector indicatorPos = (cachedSplitSegIndex == -1)
		? GetActorLocation()      // 이동 가능 범위 내 → 커서 위치
		: cachedSplitPoint;       // 이동력 초과 → 분기점에 고정

	moveDecal->SetWorldLocation(indicatorPos);
	distanceWidget->SetWorldLocation(indicatorPos + widgetOffset);
}

void ACursorIndicator::SetActiveUnit(AAllyCharacterBase* Unit)
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

		// 경로 스플라인 메쉬 재구성
		RebuildPathMeshes();

		// 데칼 위치까지의 거리 표시:
		// 범위 내 → 실제 경로 거리 / 범위 초과 → 이동력(예산) 그대로
		const float displayMeters = (cachedSplitSegIndex == -1)
			? Path->GetPathLength() / 100.f
			: activeUnit->GetCurrentMovingPoint();

		if (UMoveIndicatorWidget* Widget = Cast<UMoveIndicatorWidget>(distanceWidget->GetWidget()))
		{
			Widget->UpdateDistance(displayMeters);
		}
	}
	else
	{
		cachedPathPoints.Empty();
		pathSpline->ClearSplinePoints();
		cachedSplitSegIndex = -1;
		RebuildPathMeshes(); // 경로 없음 → 메쉬 전부 제거
	}
}

void ACursorIndicator::RebuildPathMeshes()
{
	// ─── 0. 기존 메쉬 제거 ───────────────────────────────────
	for (USplineMeshComponent* Mesh : pathMeshes)
	{
		if (IsValid(Mesh)) Mesh->DestroyComponent();
	}
	pathMeshes.Empty();

	if (!pathSegmentMesh || cachedPathPoints.Num() < 2) return;

	// ─── 1. 일정 간격으로 세분화 + 지면 스냅 ────────────────
	// NavMesh 경유점 사이를 pathSubdivisionLength(cm) 간격으로 나눠
	// 각 점을 라인 트레이스로 지면에 밀착시킴
	TArray<FVector> densePoints;
	for (int32 i = 0; i + 1 < cachedPathPoints.Num(); ++i)
	{
		const FVector& A = cachedPathPoints[i];
		const FVector& B = cachedPathPoints[i + 1];
		const int32 numSubs = FMath::Max(1, FMath::CeilToInt(FVector::Dist(A, B) / pathSubdivisionLength));
		for (int32 j = 0; j < numSubs; ++j)
		{
			densePoints.Add(SnapToGround(FMath::Lerp(A, B, (float)j / numSubs)));
		}
	}
	densePoints.Add(SnapToGround(cachedPathPoints.Last()));

	// ─── 2. 세분화된 점 기준으로 이동력 분기점 삽입 ─────────
	// 누적 거리로 예산 소진 지점을 찾아 densePoints에 삽입
	int32 visualUnreachableStart = -1;
	if (IsValid(activeUnit))
	{
		const float budgetCm = activeUnit->GetCurrentMovingPoint() * 100.f;
		float accumulated = 0.f;
		for (int32 i = 0; i + 1 < densePoints.Num(); ++i)
		{
			const float segLen = FVector::Dist(densePoints[i], densePoints[i + 1]);
			if (accumulated + segLen >= budgetCm)
			{
				const float t = (budgetCm - accumulated) / segLen;
				densePoints.Insert(SnapToGround(FMath::Lerp(densePoints[i], densePoints[i + 1], t)), i + 1);
				visualUnreachableStart = i + 1; // 이 인덱스부터 시작하는 세그먼트 → 빨간색
				break;
			}
			accumulated += segLen;
		}
	}

	// ─── 3. Catmull-Rom 접선 계산 람다 ──────────────────────
	// 이전/다음 점 방향의 평균으로 부드러운 곡선 접선 생성
	auto GetTangent = [&](int32 idx) -> FVector
	{
		const int32 last = densePoints.Num() - 1;
		if (idx <= 0)    return densePoints[1] - densePoints[0];
		if (idx >= last) return densePoints[last] - densePoints[last - 1];
		return (densePoints[idx + 1] - densePoints[idx - 1]) * 0.5f;
	};

	// ─── 4. 세그먼트마다 SplineMeshComponent 생성 ────────────
	for (int32 i = 0; i + 1 < densePoints.Num(); ++i)
	{
		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
		SplineMesh->SetStaticMesh(pathSegmentMesh);
		SplineMesh->SetMobility(EComponentMobility::Movable);
		SplineMesh->SetupAttachment(pathMeshRoot); // 절대 좌표 루트 — 월드 좌표 직접 사용
		SplineMesh->RegisterComponent();

		SplineMesh->SetStartAndEnd(densePoints[i], GetTangent(i), densePoints[i + 1], GetTangent(i + 1), false);
		SplineMesh->SetStartScale(pathMeshScale, false);
		SplineMesh->SetEndScale(pathMeshScale, true);

		const bool bUnreachable = (visualUnreachableStart != -1) && (i >= visualUnreachableStart);
		UMaterialInterface* Mat = bUnreachable ? pathMaterialUnreachable : pathMaterialReachable;
		if (Mat) SplineMesh->SetMaterial(0, Mat);

		pathMeshes.Add(SplineMesh);
	}
}

FVector ACursorIndicator::SnapToGround(const FVector& WorldPoint) const
{
	FHitResult Hit;
	const FVector Start = WorldPoint + FVector(0.f, 0.f, 500.f);
	const FVector End   = WorldPoint - FVector(0.f, 0.f, 500.f);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
	{
		return Hit.Location + FVector(0.f, 0.f, pathGroundOffset);
	}
	// 트레이스 실패 시 원래 위치 유지
	return WorldPoint;
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
