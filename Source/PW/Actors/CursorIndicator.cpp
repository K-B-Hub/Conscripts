// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/CursorIndicator.h"
#include "Characters/CharacterBase.h"
#include "WidgetComponent/MoveIndicatorWidget.h"
#include "Components/DecalComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

ACursorIndicator::ACursorIndicator()
{
	PrimaryActorTick.bCanEverTick = true;

	// 루트 컴포넌트
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// 바닥 데칼
	MoveDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("MoveDecal"));
	MoveDecal->SetupAttachment(SceneRoot);
	// 데칼은 아래를 향하도록 회전 (DecalComponent 기본 방향: -X)
	MoveDecal->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));

	// 거리 표시 위젯
	DistanceWidget = CreateDefaultSubobject<UMoveIndicatorWidget>(TEXT("DistanceWidget"));
	DistanceWidget->SetupAttachment(SceneRoot);
	// 커서 위 일정 높이에 위치
	DistanceWidget->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
	DistanceWidget->SetWidgetSpace(EWidgetSpace::Screen);
}

void ACursorIndicator::BeginPlay()
{
	Super::BeginPlay();

	// 머티리얼이 지정된 경우 데칼에 적용
	if (DecalMaterial)
	{
		MoveDecal->SetDecalMaterial(DecalMaterial);
	}
	MoveDecal->DecalSize = DecalSize;
}

void ACursorIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ─── 1. 커서 위치로 액터 이동 ────────────────────────────
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	FHitResult HitResult;
	if (PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		SetActorLocation(HitResult.Location);
	}

	// ─── 2. 경로 거리 갱신 (0.1초 쓰로틀링) ─────────────────
	PathUpdateTimer += DeltaTime;
	if (PathUpdateTimer >= PathUpdateInterval)
	{
		PathUpdateTimer = 0.f;
		UpdatePathDistance();
	}
}

void ACursorIndicator::SetActiveUnit(ACharacterBase* Unit)
{
	ActiveUnit = Unit;
}

void ACursorIndicator::UpdatePathDistance()
{
	if (!IsValid(ActiveUnit)) return;

	UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(
		GetWorld(),
		ActiveUnit->GetActorLocation(),
		GetActorLocation()
	);

	if (Path && Path->IsValid())
	{
		const float Meters = Path->GetPathLength() / 100.f;
		DistanceWidget->UpdateDistance(Meters);
	}
}
