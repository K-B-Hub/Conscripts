// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CursorIndicator.generated.h"

class UDecalComponent;
class UWidgetComponent;
class UMoveIndicatorWidget;
class AAllyCharacterBase;
class USplineComponent;
class USplineMeshComponent;

// 마우스 커서를 따라다니며 이동 목표 지점을 시각화하는 액터.
// - moveDecal: 바닥에 이동 목표 데칼 표시
// - distanceWidget: 활성 캐릭터 → 커서까지 NavMesh 경로 거리(m) 표시
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

	// 바닥에 표시되는 이동 목표 데칼
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UDecalComponent> moveDecal;

	// 이동 경로를 저장하는 스플라인 (경로 시각화 및 분기점 계산에 사용)
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USplineComponent> pathSpline;

	// 경로 메쉬 컴포넌트의 부모 — 절대 월드 좌표 고정 (액터 이동 영향 차단)
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> pathMeshRoot;

	// 경로 거리를 표시하는 위젯 컴포넌트 (Widget Class: WBP_MoveIndicator)
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UWidgetComponent> distanceWidget;

	// 거리 위젯의 위치 오프셋 (월드 공간 기준, cm)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	FVector widgetOffset = FVector(0.f, 0.f, 80.f);

	// ─── 경로 스플라인 메쉬 설정 ────────────────────────────

	// 경로 세그먼트에 사용할 스태틱 메쉬 (BP에서 지정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual|Path")
	TObjectPtr<UStaticMesh> pathSegmentMesh;

	// 이동 가능 구간 머티리얼 (흰색/초록 계열)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual|Path")
	TObjectPtr<UMaterialInterface> pathMaterialReachable;

	// 이동 불가 구간 머티리얼 (빨간색 계열)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual|Path")
	TObjectPtr<UMaterialInterface> pathMaterialUnreachable;

	// 경로 메쉬 단면 스케일 (X=폭, Y=높이). 길이는 경유점 간 거리가 결정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual|Path")
	FVector2D pathMeshScale = FVector2D(1.f, 0.05f);

	// 경로 세분화 간격 (cm). 값이 작을수록 지형 밀착도↑, 라인 트레이스 횟수↑
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual|Path")
	float pathSubdivisionLength = 10.f;

	// 지면 스냅 후 메쉬를 띄울 높이 (cm)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual|Path")
	float pathGroundOffset = 3.f;

	// ─── 인터페이스 ──────────────────────────────────────────

	// 턴이 시작된 캐릭터를 설정. BattleController에서 호출
	void SetActiveUnit(AAllyCharacterBase* Unit);

	// 마지막으로 계산된 NavMesh 경로 경유점 반환 (이동 명령에 재사용)
	const TArray<FVector>& GetCachedPathPoints() const { return cachedPathPoints; }

	// 이동 명령 직후 호출 — 현재 데칼 위치에 커서를 고정하고 경로 표시를 숨김
	void LockAtCurrentPosition();

private:
	// 현재 이동 명령 대상 캐릭터 (GC 방지를 위해 UPROPERTY 필수)
	UPROPERTY()
	TObjectPtr<AAllyCharacterBase> activeUnit = nullptr;

	// 경로 거리 계산 쓰로틀링 타이머
	float pathUpdateTimer = 0.f;
	static constexpr float pathUpdateInterval = 0.008f;

	// 마지막으로 계산된 NavMesh 경로 경유점
	TArray<FVector> cachedPathPoints;

	// 이동력 소진 분기점: 흰색/빨간색 경계 위치
	// cachedSplitSegIndex == -1 이면 경로 전체가 이동 가능 범위
	FVector cachedSplitPoint = FVector::ZeroVector;
	int32 cachedSplitSegIndex = -1;

	// 이동 중 커서 고정 모드
	bool bIsLocked = false;
	FVector lockedIndicatorPos = FVector::ZeroVector;

	// 런타임에 생성되는 경로 세그먼트 메쉬 목록 (GC 방지용 UPROPERTY)
	UPROPERTY()
	TArray<TObjectPtr<USplineMeshComponent>> pathMeshes;

	// NavMesh 경로를 계산하여 거리 위젯 갱신 + 경로 캐싱
	void UpdatePathDistance();

	// 경로 포인트 배열로 분기점을 계산하여 캐싱
	void UpdateSplitPoint();

	// 캐시된 경로 포인트로 SplineMeshComponent 배열 재구성
	void RebuildPathMeshes();

	// 월드 좌표를 지면에 스냅 (라인 트레이스)
	FVector SnapToGround(const FVector& WorldPoint) const;
};
