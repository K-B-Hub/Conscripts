// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "AllyCharacterBase.generated.h"

//플레이어가 조작하는 아군 캐릭터 베이스
UCLASS()
class PW_API AAllyCharacterBase : public ACharacterBase
{
	GENERATED_BODY()

public:
	//경로 추종 이동 및 이동력 차감 처리
	virtual void Tick(float DeltaTime) override;
	virtual void EndTurn() override;
	virtual void HandleDeath() override;

	//CursorIndicator에서 계산된 경유점 배열을 받아 순서대로 이동
	void MoveAlongPath(const TArray<FVector>& Points);

	//이동 중단
	void StopMovement();

	bool IsMoving() const { return bIsMovingToTarget; }

	//이동 자연 종료 시 브로드캐스트, 취소/턴종료 시에는 발생하지 않음
	FSimpleMulticastDelegate OnMovementCompleted;

	//목적지 도착 전 감속을 시작할 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float moveDecelRadius = 80.f;

private:
	//NavMesh 경로 경유점 및 현재 인덱스
	TArray<FVector> pathPoints;
	int32 pathPointIndex = 0;

	//정확한 스냅을 위해 최종 목적지 보관
	FVector moveDestination = FVector::ZeroVector;

	bool bIsMovingToTarget = false;

	//이전 프레임 위치, 이동 거리 실시간 차감용
	FVector lastFrameLocation = FVector::ZeroVector;
};