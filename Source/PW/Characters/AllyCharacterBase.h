// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "AllyCharacterBase.generated.h"

// 플레이어가 조작하는 아군 캐릭터 베이스.
// NavMesh 경로 추종 이동, 이동력 소진 제어 등 플레이어 이동 관련 로직을 담당.
UCLASS()
class PW_API AAllyCharacterBase : public ACharacterBase
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual void EndTurn() override;

	// ─── 이동 인터페이스 ───────────────────────────────────────

	// CursorIndicator에서 계산된 경유점 배열을 받아 순서대로 이동
	void MoveAlongPath(const TArray<FVector>& Points);

	// 이동 중단
	void StopMovement();

	bool IsMoving() const { return bIsMovingToTarget; }

private:
	// NavMesh 경로 경유점 및 현재 인덱스
	TArray<FVector> pathPoints;
	int32 pathPointIndex = 0;

	// 정확한 스냅을 위해 최종 목적지 보관
	FVector moveDestination = FVector::ZeroVector;

	bool bIsMovingToTarget = false;

	// 이전 프레임 위치 (이동 거리 실시간 차감용)
	FVector lastFrameLocation = FVector::ZeroVector;
};