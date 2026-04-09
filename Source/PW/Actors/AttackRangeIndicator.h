// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttackRangeIndicator.generated.h"

class UDecalComponent;
class USphereComponent;
class ACharacterBase;
class AAllyCharacterBase;
class UActiveSkillBase;
class ACursorIndicator;

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

	// SinglePick 모드에서 현재 스냅 중인 캐릭터 (스냅 안 됐으면 nullptr)
	ACharacterBase* GetSnappedTarget() const { return snappedTarget.Get(); }

	// 현재 인디케이터 위치가 시전자 기준 사거리 밖인지 실시간 계산
	bool ComputeOutOfRange() const;

	// 자동이동용 NavMesh 경로 반환 (CursorIndicator에 위임)
	const TArray<FVector>& GetMovePath() const;

	// 자동이동 중 위치 고정 / 해제
	void LockAtCurrentPosition();
	void Unlock();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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

	// SinglePick 모드에서 현재 스냅된 캐릭터
	TWeakObjectPtr<ACharacterBase> snappedTarget;

	// EAreaTarget 기준으로 해당 캐릭터가 스킬 적용 대상인지 확인
	bool IsAreaTarget(ACharacterBase* Character) const;

	// EPickTeam 기준으로 해당 캐릭터가 선택 대상인지 확인
	bool IsPickTarget(ACharacterBase* Character) const;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// ─── 사거리 밖 자동이동 ─────────────────────────────────────

	// CursorIndicator 클래스 (BP에서 지정 — 이동 모드와 동일 클래스)
	UPROPERTY(EditDefaultsOnly, Category = "Path")
	TSubclassOf<ACursorIndicator> cursorIndicatorClass;

	// 사거리 밖일 때 스폰된 CursorIndicator 인스턴스
	UPROPERTY()
	TObjectPtr<ACursorIndicator> movePathIndicator;

	// 사거리 밖 여부 및 이동 목표 지점
	bool bIsOutOfRange = false;
	FVector moveToPoint = FVector::ZeroVector;

	// 자동이동 중 위치 잠금
	bool bIsLocked = false;

	// CursorIndicator 생성/제거
	void SpawnMovePathIndicator();
	void DestroyMovePathIndicator();
};
