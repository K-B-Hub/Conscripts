//Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/DeploymentZone.h"
#include "Components/BoxComponent.h"

ADeploymentZone::ADeploymentZone()
{
	PrimaryActorTick.bCanEverTick = false;

	zoneBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBox"));
	SetRootComponent(zoneBox);

	//배치 판정 전용이라 캐릭터·커서 트레이스와 충돌하면 안 된다
	zoneBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	zoneBox->SetBoxExtent(FVector(500.f, 500.f, 200.f));

	//기본은 숨김, Deploy 페이즈에서 GameMode가 켠다
	zoneBox->SetHiddenInGame(true);
	zoneBox->ShapeColor = FColor(0, 160, 255);
}

bool ADeploymentZone::ContainsPoint(const FVector& point) const
{
	if (!zoneBox) return false;

	//InverseTransformPosition이 스케일을 이미 되돌리므로 스케일 미적용 범위와 비교한다
	const FVector local = zoneBox->GetComponentTransform().InverseTransformPosition(point);
	const FVector extent = zoneBox->GetUnscaledBoxExtent();

	return FMath::Abs(local.X) <= extent.X
		&& FMath::Abs(local.Y) <= extent.Y
		&& FMath::Abs(local.Z) <= extent.Z;
}

void ADeploymentZone::SetZoneVisible(bool bVisible)
{
	if (zoneBox) zoneBox->SetHiddenInGame(!bVisible);
}