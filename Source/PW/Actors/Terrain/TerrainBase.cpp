//Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Terrain/TerrainBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/TerrainComponent.h"
#include "GameMode/BattleGameMode.h"

ATerrainBase::ATerrainBase()
{
	//체류 중 매 프레임 처리는 캐릭터의 TerrainComponent가 대신 디스패치, 지형 자체는 틱 불필요
	PrimaryActorTick.bCanEverTick = false;

	terrainBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TerrainBox"));
	SetRootComponent(terrainBox);
	terrainBox->SetBoxExtent(FVector(200.f, 200.f, 100.f));
	terrainBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	terrainBox->SetCollisionObjectType(ECC_WorldStatic);
	terrainBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	terrainBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	//지형은 통행을 막지 않음, NavMesh에도 영향 없음
	terrainBox->SetCanEverAffectNavigation(false);

	terrainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TerrainMesh"));
	terrainMesh->SetupAttachment(terrainBox);
	terrainMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	terrainMesh->SetCanEverAffectNavigation(false);

	terrainVfx = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TerrainVfx"));
	terrainVfx->SetupAttachment(terrainBox);
}

void ATerrainBase::BeginPlay()
{
	Super::BeginPlay();

	terrainBox->OnComponentBeginOverlap.AddDynamic(this, &ATerrainBase::OnBoxBeginOverlap);
	terrainBox->OnComponentEndOverlap.AddDynamic(this, &ATerrainBase::OnBoxEndOverlap);

	//바인딩 전부터 겹쳐 있던 캐릭터는 BeginOverlap이 오지 않으므로 직접 수집
	//지형 위에서 전투 시작, 캐릭터를 덮는 위치에 지형 스폰이 이 경로
	TArray<AActor*> overlappingActors;
	terrainBox->GetOverlappingActors(overlappingActors, ACharacterBase::StaticClass());
	for (AActor* actor : overlappingActors)
	{
		OnBoxBeginOverlap(terrainBox, actor, nullptr, 0, false, FHitResult());
	}

	//AI 경로 평가가 액터 순회 없이 지형을 조회하도록 등록
	if (ABattleGameMode* GM = GetWorld()->GetAuthGameMode<ABattleGameMode>())
	{
		GM->RegisterTerrain(this);
	}
}

void ATerrainBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//남아 있는 체류자의 스탯 델타를 되돌려 지형 소멸 후 잔여 효과 방지
	for (int32 i = occupants.Num() - 1; i >= 0; --i)
	{
		if (ACharacterBase* character = occupants[i].Get())
		{
			if (UTerrainComponent* TC = character->GetTerrainComponent())
			{
				TC->ExitTerrain(this);
			}
		}
	}
	occupants.Empty();

	if (ABattleGameMode* GM = GetWorld()->GetAuthGameMode<ABattleGameMode>())
	{
		GM->UnregisterTerrain(this);
	}

	Super::EndPlay(EndPlayReason);
}

bool ATerrainBase::IsLocationInside(const FVector& location) const
{
	if (!terrainBox) return false;

	//박스 로컬 좌표로 변환해 회전까지 반영한 판정
	const FVector localLocation = terrainBox->GetComponentTransform().InverseTransformPosition(location);
	const FVector extent = terrainBox->GetScaledBoxExtent();

	return FMath::Abs(localLocation.X) <= extent.X
		&& FMath::Abs(localLocation.Y) <= extent.Y
		&& FMath::Abs(localLocation.Z) <= extent.Z;
}

void ATerrainBase::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacterBase* character = Cast<ACharacterBase>(OtherActor);
	if (!character || character->IsDead()) return;

	//캡슐 외 컴포넌트의 중복 오버렙 무시
	if (occupants.Contains(character)) return;

	occupants.Add(character);

	if (UTerrainComponent* TC = character->GetTerrainComponent())
	{
		TC->EnterTerrain(this);
	}
}

void ATerrainBase::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacterBase* character = Cast<ACharacterBase>(OtherActor);
	if (!character) return;

	if (occupants.Remove(character) == 0) return;

	if (UTerrainComponent* TC = character->GetTerrainComponent())
	{
		TC->ExitTerrain(this);
	}
}