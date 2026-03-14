// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/CursorIndicator.h"
#include "Characters/CharacterBase.h"
#include "Widget/MoveIndicatorWidget.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

ACursorIndicator::ACursorIndicator()
{
	PrimaryActorTick.bCanEverTick = true;

	// 루트 컴포넌트
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

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
}

void ACursorIndicator::SetActiveUnit(ACharacterBase* Unit)
{
	activeUnit = Unit;
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
		const float Meters = Path->GetPathLength() / 100.f;

		// UWidgetComponent가 호스팅하는 위젯을 UMoveIndicatorWidget으로 캐스팅
		if (UMoveIndicatorWidget* Widget = Cast<UMoveIndicatorWidget>(distanceWidget->GetWidget()))
		{
			Widget->UpdateDistance(Meters);
		}
	}
}
