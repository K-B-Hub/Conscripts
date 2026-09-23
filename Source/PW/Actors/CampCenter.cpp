//Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/CampCenter.h"
#include "Components/StaticMeshComponent.h"

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