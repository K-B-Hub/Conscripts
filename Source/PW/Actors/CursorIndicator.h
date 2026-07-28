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

//마우스 커서를 따라다니며 이동 목표 지점을 시각화하는 액터
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

	//바닥에 표시되는 이동 목표 데칼
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UDecalComponent> moveDecal;

	//이동 경로를 저장하는 스플라인
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USplineComponent> pathSpline;

	//경로 메쉬 컴포넌트의 부모, 절대 월드 좌표 고정
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> pathMeshRoot;

	//경로 거리를 표시하는 위젯 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UWidgetComponent> distanceWidget;

	//거리 위젯의 위치 오프셋
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	FVector widgetOffset = FVector(0.f, 0.f, 80.f);

	//경로 세그먼트에 사용할 스태틱 메쉬
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual|Path")
	TObjectPtr<UStaticMesh> pathSegmentMesh;

	//이동 가능 구간 머티리얼
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual|Path")
	TObjectPtr<UMaterialInterface> pathMaterialReachable;

	//이동 불가 구간 머티리얼
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual|Path")
	TObjectPtr<UMaterialInterface> pathMaterialUnreachable;

	//지형 통과 구간 머티리얼, 미지정 시 일반 구간과 같은 색으로 표시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual|Path")
	TObjectPtr<UMaterialInterface> pathMaterialTerrain;

	//경로 메쉬 단면 스케일
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual|Path")
	FVector2D pathMeshScale = FVector2D(1.f, 0.05f);

	//경로 세분화 간격
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual|Path")
	float pathSubdivisionLength = 10.f;

	//지면 스냅 후 메쉬를 띄울 높이
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual|Path")
	float pathGroundOffset = 3.f;

	//턴이 시작된 캐릭터 설정
	void SetActiveUnit(AAllyCharacterBase* Unit);

	//마지막으로 계산된 NavMesh 경로 경유점 반환
	const TArray<FVector>& GetCachedPathPoints() const { return cachedPathPoints; }

	//현재 위치에 커서를 고정
	void LockAtCurrentPosition();

	//고정 해제
	void Unlock();

	//외부에서 목표 위치 직접 지정
	void SetTargetOverride(const FVector& Target);
	//목표 위치 지정 해제
	void ClearTargetOverride();

private:
	//현재 이동 명령 대상 캐릭터
	TWeakObjectPtr<AAllyCharacterBase> activeUnit = nullptr;

	//경로 거리 계산 쓰로틀링 타이머
	float pathUpdateTimer = 0.f;
	static constexpr float pathUpdateInterval = 0.008f;

	//마지막으로 계산된 NavMesh 경로 경유점
	TArray<FVector> cachedPathPoints;

	//이동력 소진 분기점 위치, -1이면 경로 전체가 이동 가능
	FVector cachedSplitPoint = FVector::ZeroVector;
	int32 cachedSplitSegIndex = -1;

	//시작점부터 분기점까지의 실제 이동 거리(cm), 지형 배율이 걸리면 소모 이동력과 다름
	float cachedSplitDistanceCm = 0.f;

	//이동 중 커서 고정 모드
	bool bIsLocked = false;
	FVector lockedIndicatorPos = FVector::ZeroVector;

	//외부 위치 지정 모드
	bool bHasTargetOverride = false;
	FVector targetOverride = FVector::ZeroVector;

	//런타임에 생성되는 경로 세그먼트 메쉬 목록
	UPROPERTY()
	TArray<TObjectPtr<USplineMeshComponent>> pathMeshes;

	//NavMesh 경로를 계산하여 거리 위젯 갱신 및 경로 캐싱
	void UpdatePathDistance();

	//경로 포인트 배열로 분기점 계산
	void UpdateSplitPoint();

	//캐시된 경로 포인트로 SplineMeshComponent 배열 재구성
	void RebuildPathMeshes();

	//월드 좌표를 지면에 스냅
	FVector SnapToGround(const FVector& WorldPoint) const;

	//지정 지점의 지형 이동력 소모 배율, 지형 밖이면 1.0
	//bOutInTerrain은 배율과 무관하게 지형 내부인지 여부
	float GetTerrainCostMultiplierAt(const FVector& WorldPoint, bool& bOutInTerrain) const;
};
