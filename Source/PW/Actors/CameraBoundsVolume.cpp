//Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/CameraBoundsVolume.h"
#include "Components/BoxComponent.h"

ACameraBoundsVolume::ACameraBoundsVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	boundsBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoundsBox"));
	SetRootComponent(boundsBox);

	//범위 표시 전용, 트레이스·캐릭터와 충돌하면 안 된다
	boundsBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	boundsBox->SetBoxExtent(FVector(5000.f, 5000.f, 500.f));

	//에디터에서만 보인다
	boundsBox->SetHiddenInGame(true);
	boundsBox->ShapeColor = FColor(255, 200, 0);
}
