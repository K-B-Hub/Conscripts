//Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/CampSpawnPoint.h"
#include "Components/BoxComponent.h"

ACampSpawnPoint::ACampSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	hoverBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HoverBox"));
	SetRootComponent(hoverBox);

	//캐릭터 한 명을 감싸는 정도, 자리마다 에디터에서 조정 가능
	hoverBox->SetBoxExtent(FVector(60.f, 60.f, 100.f));
	hoverBox->SetHiddenInGame(true);

	//커서 트레이스에만 반응하고 캐릭터 이동·물리에는 관여하지 않는다
	hoverBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	hoverBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	hoverBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}