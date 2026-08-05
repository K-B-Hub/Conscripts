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
#include "AI/AINavigationHelper.h"
#include "Actors/Terrain/TerrainBase.h"

ACursorIndicator::ACursorIndicator()
{
	PrimaryActorTick.bCanEverTick = true;

	//루트 컴포넌트
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	//경로 스플라인, 접선 계산 전용
	pathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	pathSpline->SetupAttachment(SceneRoot);
	pathSpline->SetDrawDebug(false);

	//경로 메쉬 루트, 절대 월드 좌표 고정
	pathMeshRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PathMeshRoot"));
	pathMeshRoot->SetupAttachment(SceneRoot);
	pathMeshRoot->SetAbsolute(true, true, true);

	//바닥 데칼
	moveDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("MoveDecal"));
	moveDecal->SetupAttachment(SceneRoot);
	//데칼이 아래를 향하도록 회전
	moveDecal->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));

	//거리 표시 위젯 컴포넌트
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

	//이동 완료 감지 시 잠금 자동 해제
	if (bIsLocked)
	{
		if (!activeUnit.IsValid() || !activeUnit->IsMoving())
		{
			bIsLocked = false;
		}
		else
		{
			//이동 중이면 데칼과 위젯을 고정 위치에 유지하고 종료
			moveDecal->SetWorldLocation(lockedIndicatorPos);
			distanceWidget->SetWorldLocation(lockedIndicatorPos + widgetOffset);
			return;
		}
	}

	//액터 위치 갱신, 외부 지정 또는 커서 추적
	if (bHasTargetOverride)
	{
		SetActorLocation(targetOverride);
	}
	else
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (!PC) return;

		FHitResult HitResult;
		if (PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
		{
			SetActorLocation(HitResult.Location);
		}
	}

	//경로 거리 갱신, 쓰로틀링 적용
	pathUpdateTimer += DeltaTime;
	if (pathUpdateTimer >= pathUpdateInterval)
	{
		pathUpdateTimer = 0.f;
		UpdatePathDistance();
	}

	//데칼과 위젯 위치는 이동 가능 범위 경계까지만 따라오기
	const FVector indicatorPos = (cachedSplitSegIndex == -1)
		? GetActorLocation()
		: cachedSplitPoint;

	moveDecal->SetWorldLocation(indicatorPos);
	distanceWidget->SetWorldLocation(indicatorPos + widgetOffset);
}

void ACursorIndicator::SetActiveUnit(AAllyCharacterBase* Unit)
{
	activeUnit = Unit;
}

void ACursorIndicator::LockAtCurrentPosition()
{
	//이동 명령 시점의 데칼 위치를 고정 위치로 저장
	lockedIndicatorPos = (cachedSplitSegIndex == -1)
		? GetActorLocation()
		: cachedSplitPoint;
	bIsLocked = true;
}

void ACursorIndicator::Unlock()
{
	bIsLocked = false;
}

void ACursorIndicator::SetTargetOverride(const FVector& Target)
{
	bHasTargetOverride = true;
	targetOverride = Target;
}

void ACursorIndicator::ClearTargetOverride()
{
	bHasTargetOverride = false;
}

void ACursorIndicator::UpdatePathDistance()
{
	if (!activeUnit.IsValid()) return;

	UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(
		GetWorld(),
		activeUnit->GetActorLocation(),
		GetActorLocation()
	);

	if (Path && Path->IsValid())
	{
		//경로 경유점 캐싱
		cachedPathPoints.Empty();
		for (const FNavPathPoint& Point : Path->GetPath()->GetPathPoints())
		{
			cachedPathPoints.Add(Point.Location);
		}

		//스플라인 포인트 갱신
		pathSpline->ClearSplinePoints(false);
		for (const FVector& Point : cachedPathPoints)
		{
			pathSpline->AddSplinePoint(Point, ESplineCoordinateSpace::World, false);
		}
		pathSpline->UpdateSpline();

		//이동력 분기점 계산
		UpdateSplitPoint();

		//경로 스플라인 메쉬 재구성
		RebuildPathMeshes();

		//양쪽 모두 실제 이동 거리, 범위 초과 시에는 분기점까지의 거리
		//지형 배율이 걸리면 소모 이동력과 값이 달라지므로 거리만 표시
		const float displayMeters = (cachedSplitSegIndex == -1)
			? Path->GetPathLength() / 100.f
			: cachedSplitDistanceCm / 100.f;

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
		RebuildPathMeshes(); //경로 없음, 메쉬 전부 제거
	}
}

void ACursorIndicator::RebuildPathMeshes()
{
	//메쉬 에셋 없거나 경로 없음 → 풀 비우고 종료
	if (!pathSegmentMesh || cachedPathPoints.Num() < 2)
	{
		for (USplineMeshComponent* Mesh : pathMeshes)
		{
			if (IsValid(Mesh)) Mesh->DestroyComponent();
		}
		pathMeshes.Empty();
		return;
	}

	//경유점 사이를 일정 간격으로 세분화하고 각 점을 지면에 스냅
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

	//지형 배율을 반영한 누적 비용으로 이동력 예산 소진 지점을 찾아 densePoints에 삽입
	int32 visualUnreachableStart = -1;
	if (activeUnit.IsValid())
	{
		const float budgetCm = activeUnit->GetCurrentMovingPoint() * 100.f / activeUnit->GetStanceMoveCostMultiplier(); //자세 배율(포복 등) 반영
		float accumulated = 0.f;
		for (int32 i = 0; i + 1 < densePoints.Num(); ++i)
		{
			const float segLen = FVector::Dist(densePoints[i], densePoints[i + 1]);

			bool bInTerrain = false;
			const float multiplier = GetTerrainCostMultiplierAt(
				(densePoints[i] + densePoints[i + 1]) * 0.5f, bInTerrain);
			const float segCost = segLen * multiplier;

			if (accumulated + segCost >= budgetCm)
			{
				const float t = (budgetCm - accumulated) / segCost;
				densePoints.Insert(SnapToGround(FMath::Lerp(densePoints[i], densePoints[i + 1], t)), i + 1);
				visualUnreachableStart = i + 1; //이 인덱스부터 빨간색
				break;
			}
			accumulated += segCost;
		}
	}

	const int32 neededSegments = densePoints.Num() - 1;

	//세그먼트 수가 바뀔 때만 컴포넌트 생성/파괴, 평소엔 트랜스폼만 갱신
	while (pathMeshes.Num() < neededSegments)
	{
		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
		SplineMesh->SetStaticMesh(pathSegmentMesh);
		SplineMesh->SetMobility(EComponentMobility::Movable);
		SplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SplineMesh->SetupAttachment(pathMeshRoot); //절대 좌표 루트에 부착해 월드 좌표 직접 사용
		SplineMesh->RegisterComponent();
		pathMeshes.Add(SplineMesh);
	}
	while (pathMeshes.Num() > neededSegments)
	{
		if (USplineMeshComponent* Mesh = pathMeshes.Pop())
		{
			if (IsValid(Mesh)) Mesh->DestroyComponent();
		}
	}

	//Catmull-Rom 접선 계산, 이전/다음 점 방향의 평균
	auto GetTangent = [&](int32 idx) -> FVector
	{
		const int32 last = densePoints.Num() - 1;
		if (idx <= 0)    return densePoints[1] - densePoints[0];
		if (idx >= last) return densePoints[last] - densePoints[last - 1];
		return (densePoints[idx + 1] - densePoints[idx - 1]) * 0.5f;
	};

	//풀링된 세그먼트마다 트랜스폼·재질만 갱신
	for (int32 i = 0; i < neededSegments; ++i)
	{
		USplineMeshComponent* SplineMesh = pathMeshes[i];
		if (!IsValid(SplineMesh)) continue;

		SplineMesh->SetStartAndEnd(densePoints[i], GetTangent(i), densePoints[i + 1], GetTangent(i + 1), true);
		SplineMesh->SetStartScale(pathMeshScale, false);
		SplineMesh->SetEndScale(pathMeshScale, true);

		//우선순위: 이동 불가 > 지형 통과 > 일반
		const bool bUnreachable = (visualUnreachableStart != -1) && (i >= visualUnreachableStart);

		UMaterialInterface* Mat = pathMaterialReachable;
		if (bUnreachable)
		{
			Mat = pathMaterialUnreachable;
		}
		else if (pathMaterialTerrain)
		{
			bool bInTerrain = false;
			GetTerrainCostMultiplierAt((densePoints[i] + densePoints[i + 1]) * 0.5f, bInTerrain);
			if (bInTerrain) Mat = pathMaterialTerrain;
		}

		if (Mat) SplineMesh->SetMaterial(0, Mat);
	}
}

float ACursorIndicator::GetTerrainCostMultiplierAt(const FVector& WorldPoint, bool& bOutInTerrain) const
{
	TArray<TObjectPtr<ATerrainBase>> here;
	UAINavigationHelper::GetTerrainsAt(GetWorld(), WorldPoint, here);

	bOutInTerrain = here.Num() > 0;

	//겹친 지형은 배율을 곱산, TerrainComponent와 동일한 규칙
	float multiplier = 1.f;
	for (ATerrainBase* terrain : here)
	{
		if (terrain) multiplier *= terrain->movingPointCostMultiplier;
	}
	return multiplier;
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
	//트레이스 실패 시 원래 위치 유지
	return WorldPoint;
}

void ACursorIndicator::UpdateSplitPoint()
{
	cachedSplitSegIndex = -1;
	cachedSplitDistanceCm = 0.f;

	if (!activeUnit.IsValid() || cachedPathPoints.Num() < 2) return;

	//현재 이동력을 cm으로 변환
	const float budgetCm = activeUnit->GetCurrentMovingPoint() * 100.f / activeUnit->GetStanceMoveCostMultiplier(); //자세 배율(포복 등) 반영
	float accumulated = 0.f;

	//소모 이동력과 별개로 실제 이동 거리도 누적, 거리 위젯 표시용
	float accumulatedDistCm = 0.f;

	for (int32 i = 0; i + 1 < cachedPathPoints.Num(); ++i)
	{
		const FVector& A = cachedPathPoints[i];
		const FVector& B = cachedPathPoints[i + 1];
		const float segLen = FVector::Dist(A, B);
		if (segLen <= KINDA_SMALL_NUMBER) continue;

		//RebuildPathMeshes와 같은 세분화 간격을 써야 데칼 위치와 색 경계가 일치
		const int32 stepCount = FMath::Max(1, FMath::CeilToInt(segLen / pathSubdivisionLength));
		const float stepLen = segLen / stepCount;

		for (int32 s = 0; s < stepCount; ++s)
		{
			const FVector subA = FMath::Lerp(A, B, static_cast<float>(s) / stepCount);
			const FVector subB = FMath::Lerp(A, B, static_cast<float>(s + 1) / stepCount);

			bool bInTerrain = false;
			const float stepCost = stepLen * GetTerrainCostMultiplierAt((subA + subB) * 0.5f, bInTerrain);

			if (accumulated + stepCost >= budgetCm)
			{
				//이 조각 안에서 이동력 소진, 비용 비율로 분기점 보간
				const float t = (budgetCm - accumulated) / stepCost;
				cachedSplitPoint = FMath::Lerp(subA, subB, t);
				cachedSplitSegIndex = i;
				//배율이 걸린 조각이라 남은 예산이 곧 거리가 아님, 보간 비율로 실제 거리 산출
				cachedSplitDistanceCm = accumulatedDistCm + stepLen * t;
				return;
			}
			accumulated += stepCost;
			accumulatedDistCm += stepLen;
		}
	}
	//누적 거리가 예산 미만이면 경로 전체가 이동 가능
}
