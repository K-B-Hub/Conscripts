// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttackRangeIndicator.generated.h"

class UDecalComponent;
class USphereComponent;
class ACharacterBase;
class UActiveSkillBase;

// 마우스를 따라다니며 스킬 영향 범위를 표시하는 액터
// Skill 객체를 직접 참조해 자체 초기화하고, 오버랩 시 전투 예측 위젯을 표시
// ESelectMode::Self 이면 시전자 위치에 고정
UCLASS()
class PW_API AAttackRangeIndicator : public AActor
{
	GENERATED_BODY()

public:
	AAttackRangeIndicator();

	// Skill 객체로 인디케이터 초기화 — 범위 형태/파라미터를 Skill에서 직접 읽음
	void InitIndicator(ACharacterBase* Caster, UActiveSkillBase* Skill);

	const TArray<ACharacterBase*>& GetOverlappingTargets() const { return overlappingTargets; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> rootScene;

	// 범위 공격 데칼 (단일 대상일 때 비활성)
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDecalComponent> areaDecal;

	// 오버랩 감지용 구체 콜리전
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> overlapSphere;

	// 범위 공격 데칼 머티리얼 (BP에서 지정)
	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	TObjectPtr<UMaterialInterface> areaDecalMaterial;

	// 데칼 투영 깊이 (cm)
	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	float decalProjectionDepth = 500.f;

	// 시전자 (사거리 제한 및 Self 고정 위치용)
	TWeakObjectPtr<ACharacterBase> caster;
	float pickRange = 0.f;

	// 범위 공격 여부
	bool bIsAreaAttack = false;

	// true: 시전자 위치에 고정 (ESelectMode::Self) — Tick에서 마우스 추적 생략
	bool bFixedAtCaster = false;

	// 사용 중인 스킬 참조
	UPROPERTY()
	TObjectPtr<UActiveSkillBase> cachedSkill;

	// 오버랩 중인 캐릭터 목록
	TArray<ACharacterBase*> overlappingTargets;

	// EAreaTarget 기준으로 해당 캐릭터가 스킬 적용 대상인지 확인
	bool IsAreaTarget(ACharacterBase* Character) const;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};