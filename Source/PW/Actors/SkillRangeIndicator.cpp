// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/SkillRangeIndicator.h"
#include "Components/DecalComponent.h"

ASkillRangeIndicator::ASkillRangeIndicator()
{
	PrimaryActorTick.bCanEverTick = false;

	rootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = rootScene;

	rangeDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("RangeDecal"));
	rangeDecal->SetupAttachment(RootComponent);
	//데칼을 지면 방향으로 투영
	rangeDecal->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
}

void ASkillRangeIndicator::BeginPlay()
{
	Super::BeginPlay();

	if (rangeDecalMaterial)
	{
		rangeDecal->SetDecalMaterial(rangeDecalMaterial);
	}
}

void ASkillRangeIndicator::InitRange(float Range)
{
	//DecalSize의 X는 투영 깊이, Y와 Z는 반지름
	rangeDecal->DecalSize = FVector(decalProjectionDepth, Range, Range);
	rangeDecal->MarkRenderStateDirty();
}