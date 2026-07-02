// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "EnemyBase.generated.h"

class AAllyCharacterBase;
class AEnemyAIController;
class UUtilityAIComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyDeath, AEnemyBase*, DeadEnemy, AAllyCharacterBase*, Killer);

UCLASS()
class PW_API AEnemyBase : public ACharacterBase
{
	GENERATED_BODY()

public:
	AEnemyBase();

	//사망 시 경험치 분배용 델리게이트
	FOnEnemyDeath OnEnemyDeath;

	//경험치 분배 브로드캐스트
	virtual void HandleDeath() override;
	//비전투 감지 부채꼴 디버그 표시
	virtual void Tick(float DeltaTime) override;

	//적 턴 시작 시 AIController에 통지
	virtual void InitTurn() override;

	//해당 좌표가 전방 감지 부채꼴 안인지 확인, 시야 차단 검사는 AIController 책임
	bool IsInDetectionFan(const FVector& worldPoint) const;

	float GetDetectionRadius() const { return detectionRadius; }
	float GetDetectionAngle() const { return detectionAngle; }

	//왕복 순찰 가능한지 — BeginPlay에서 원위치를 더하므로 입력 지점이 1개 이상이면 총 2개 이상
	bool CanPatrol() const { return patrolPoints.Num() >= 2; }
	//현재 순찰 목표의 월드 좌표 (스폰위치 + 상대좌표)
	FVector GetCurrentPatrolWorldLocation() const;
	//다음 순찰 지점으로 인덱스 진행
	void AdvancePatrolIndex();

protected:
	virtual void BeginPlay() override;

	//비전투 순찰 지점 — 스폰 위치 기준 상대좌표. 배치한 각 적마다 개별 설정.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Patrol")
	TArray<FVector> patrolPoints;

	//BeginPlay 시점의 배치 위치 — 상대좌표의 기준점
	FVector spawnLocation;

	//현재 향하는 순찰 지점 인덱스 — 턴을 넘어 보존
	int32 patrolIndex = 0;

	//왕복 진행 방향 — true면 역방향(되돌아가는 중). 양 끝점에서 토글
	bool bPatrolReverse = false;

	//부채꼴 반경
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection")
	float detectionRadius = 1200.f;

	//부채꼴 전체 각도, 전방 기준 좌우 각각 절반씩
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection", meta = (ClampMin = "0.0", ClampMax = "360.0"))
	float detectionAngle = 120.f;

	//에디터에서 부채꼴 디버그 표시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection")
	bool bShowDetectionDebug = false;

	//전투 행동 결정, 전투 진입 후 자기 턴에 ExecuteTurn 호출
	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UUtilityAIComponent> utilityAI;
};
