//Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/SquadSpawnPoint.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"

ASquadSpawnPoint::ASquadSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	//대형이 앵커 정면을 기준으로 펼쳐지므로 에디터에서 방향이 보여야 한다
	//ArrowComponent는 에디터 전용이라 게임 화면에는 나오지 않는다
	UArrowComponent* arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Direction"));
	SetRootComponent(arrow);

#if WITH_EDITORONLY_DATA
	//화면 크기 고정이라 멀리서도 같은 크기로 보인다
	arrow->ArrowSize = 3.f;
	arrow->bIsScreenSizeScaled = true;
	arrow->SetArrowColor(FLinearColor::Red);

	//맵 전체를 내려다볼 때 위치를 찾는 아이콘
	UBillboardComponent* sprite = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	if (sprite)
	{
		static ConstructorHelpers::FObjectFinderOptional<UTexture2D> icon(TEXT("/Engine/EditorMaterials/TargetIcon"));
		sprite->Sprite = icon.Get();
		sprite->bIsScreenSizeScaled = true;
		sprite->SetupAttachment(arrow);
	}
#endif
}