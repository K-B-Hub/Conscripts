//Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/CampCenter.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ACampCenter::ACampCenter()
{
	PrimaryActorTick.bCanEverTick = false;

	campMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CampMesh"));
	SetRootComponent(campMesh);

	//클릭 감지를 위해 커서 트레이스 채널을 막는다
	campMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	campMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	campMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void ACampCenter::BeginPlay()
{
	Super::BeginPlay();

	//강조는 머티리얼이 지정됐을 때만, 미지정이면 오버레이를 깔지 않는다
	if (!highlightOverlayMaterial) return;

	highlightMID = UMaterialInstanceDynamic::Create(highlightOverlayMaterial, this);
	campMesh->SetOverlayMaterial(highlightMID);

	//평소에도 약하게 켜둬 클릭 대상임을 알린다
	SetHovered(false);
}

void ACampCenter::SetHovered(bool bHovered)
{
	if (!highlightMID) return;

	highlightMID->SetScalarParameterValue(intensityParameterName,
		bHovered ? hoveredIntensity : idleIntensity);
}
