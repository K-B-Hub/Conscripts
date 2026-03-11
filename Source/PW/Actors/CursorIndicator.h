// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CursorIndicator.generated.h"

class UDecalComponent;
class UMoveIndicatorWidget;
class ACharacterBase;

/**
 * 마우스 커서를 따라다니며 이동 목표 지점을 시각화하는 액터.
 * - MoveDecal: 바닥에 이동 목표 데칼 표시
 * - DistanceWidget: 활성 캐릭터 → 커서까지 NavMesh 경로 거리(m) 표시
 */
UCLASS()
class PW_API ACursorIndicator : public AActor
{
	GENERATED_BODY()

public:
	ACursorIndicator();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// ─── 컴포넌트 ────────────────────────────────────────────

	/** 바닥에 표시되는 이동 목표 데칼 */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UDecalComponent> MoveDecal;

	/** 경로 거리를 표시하는 위젯 컴포넌트 */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UMoveIndicatorWidget> DistanceWidget;

	// ─── 비주얼 설정 (BP에서 할당) ───────────────────────────

	/** 데칼에 적용할 머티리얼 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UMaterialInterface> DecalMaterial;

	/** 데칼 크기 (X: 깊이, Y/Z: 너비/높이) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	FVector DecalSize = FVector(5.f, 50.f, 50.f);

	// ─── 인터페이스 ──────────────────────────────────────────

	/** 턴이 시작된 캐릭터를 설정. BattleController에서 호출 */
	void SetActiveUnit(ACharacterBase* Unit);

private:
	// 현재 이동 명령 대상 캐릭터 (GC 방지를 위해 UPROPERTY 필수)
	UPROPERTY()
	TObjectPtr<ACharacterBase> ActiveUnit = nullptr;

	// 경로 거리 계산 쓰로틀링 타이머
	float PathUpdateTimer = 0.f;
	static constexpr float PathUpdateInterval = 0.1f;

	/** NavMesh 경로 거리를 계산하여 DistanceWidget을 갱신 */
	void UpdatePathDistance();
};
