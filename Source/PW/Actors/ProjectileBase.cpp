// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/ProjectileBase.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"

AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;

	projectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	//비행 중 충돌·오버랩은 쓰지 않음, 효과는 도착 시점에만 적용
	projectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = projectileMesh;

	//트레일은 메쉬를 따라다니며 비행 중 자동 재생, 시스템 미지정이면 아무 일도 하지 않음
	trailFx = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailFx"));
	trailFx->SetupAttachment(projectileMesh);
}

void AProjectileBase::Launch(const TArray<FVector>& Path, float Speed)
{
	//경로나 속도가 없으면 타이밍을 잡을 수 없으니 즉시 도착 처리
	if (Path.Num() < 2 || Speed <= 0.f)
	{
		Arrive();
		return;
	}

	pathPoints = Path;
	pathIndex = 1; //0은 출발점
	flightSpeed = Speed;
	SetActorLocation(pathPoints[0]);
}

void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bArrived || pathPoints.Num() < 2) return;

	//이번 프레임 이동량을 샘플 단위로 소비, 속도가 빠르면 한 프레임에 여러 샘플 통과
	float remain = flightSpeed * DeltaTime;
	while (remain > 0.f && pathPoints.IsValidIndex(pathIndex))
	{
		const FVector cur = GetActorLocation();
		const FVector delta = pathPoints[pathIndex] - cur;
		const float dist = delta.Size();

		if (bOrientToVelocity && dist > KINDA_SMALL_NUMBER)
		{
			SetActorRotation(delta.Rotation());
		}

		if (dist <= remain)
		{
			//샘플 도달, 남은 이동량으로 다음 샘플 계속
			SetActorLocation(pathPoints[pathIndex]);
			remain -= dist;
			++pathIndex;
		}
		else
		{
			SetActorLocation(cur + delta / dist * remain);
			remain = 0.f;
		}
	}

	//마지막 샘플까지 통과하면 도착
	if (!pathPoints.IsValidIndex(pathIndex))
	{
		Arrive();
	}
}

void AProjectileBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//도착 전 외부에서 제거된 경우에도 커밋 유실 방지, 레벨 종료는 제외
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		Arrive();
	}

	Super::EndPlay(EndPlayReason);
}

void AProjectileBase::Arrive()
{
	if (bArrived) return;
	bArrived = true;

	onArrived.ExecuteIfBound();
	Destroy();
}