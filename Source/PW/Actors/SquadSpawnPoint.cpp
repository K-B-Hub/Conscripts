//Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/SquadSpawnPoint.h"
#include "Components/ArrowComponent.h"

ASquadSpawnPoint::ASquadSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	//대형이 앵커 정면을 기준으로 펼쳐지므로 에디터에서 방향이 보여야 한다
	//ArrowComponent는 에디터 전용이라 게임 화면에는 나오지 않는다
	UArrowComponent* arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Direction"));
	SetRootComponent(arrow);

#if WITH_EDITORONLY_DATA
	arrow->ArrowSize = 2.f;
	arrow->SetArrowColor(FLinearColor::Red);
#endif
}