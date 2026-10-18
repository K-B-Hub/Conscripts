// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

class UStaticMeshComponent;
class UNiagaraComponent;

//투사체 도착 통지, 스킬 효과 커밋 시점을 잡는다
DECLARE_DELEGATE(FOnProjectileArrived);

//스킬 효과의 적용 타이밍을 잡는 투사체, 미리 계산된 폴리라인을 등속으로 따라간다
//메쉬·이펙트는 파생 BP에서 지정, 비행 중 충돌 판정은 없다(대상은 발사 전에 확정됨)
UCLASS(Blueprintable)
class PW_API AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	AProjectileBase();

	//경로·속도를 받아 비행 시작, 마지막 점 도달 시 onArrived 통지 후 자기 제거
	void Launch(const TArray<FVector>& Path, float Speed);

	//도착 통지, SkillComponent가 커밋에 연결
	FOnProjectileArrived onArrived;

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//투사체 외형, 메쉬 에셋은 BP에서 지정
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> projectileMesh;

	//비행 궤적 트레일, 나이아가라 시스템 에셋은 BP에서 지정
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UNiagaraComponent> trailFx;

	//진행 방향으로 기수를 정렬할지, 구체 등 방향 없는 투사체는 끈다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	bool bOrientToVelocity = true;

private:
	//비행 경로 폴리라인과 현재 목표 샘플 인덱스
	TArray<FVector> pathPoints;
	int32 pathIndex = 0;

	//비행 속도(cm/s)
	float flightSpeed = 0.f;

	//도착 통지 1회 보장
	bool bArrived = false;

	//도착 처리, 통지 후 자기 제거
	void Arrive();
};