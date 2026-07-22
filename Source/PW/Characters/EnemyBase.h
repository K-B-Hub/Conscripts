// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "EnemyBase.generated.h"

class AAllyCharacterBase;
class AEnemyAIController;
class UUtilityAIComponent;
class UUpgradeTableData;

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
	float GetProximityWaveRadius() const { return proximityWaveRadius; }

	//플레이어 진영 시야 내 여부, 시야 밖 적은 숨김·카메라 추적 제외
	bool IsVisibleToPlayers() const { return bVisibleToPlayers; }

	//보스 유닛 여부, 경험치 유닛 계수(보스 2 / 일반 1) 판정용
	bool IsBoss() const { return bIsBoss; }

	//비전투 중 상대 진영 피격 시 즉시 전투 전환
	virtual void ReceiveDamage(int32 Amount, bool bIsLethal) override;

	//왕복 순찰 가능한지 — BeginPlay에서 원위치를 더하므로 입력 지점이 1개 이상이면 총 2개 이상
	bool CanPatrol() const { return patrolPoints.Num() >= 2; }
	//현재 순찰 목표의 월드 좌표 (스폰위치 + 상대좌표)
	FVector GetCurrentPatrolWorldLocation() const;
	//다음 순찰 지점으로 인덱스 진행
	void AdvancePatrolIndex();

protected:
	virtual void BeginPlay() override;

	//레벨업 시 선택 없이 랜덤 강화 1개를 자동 습득
	virtual void GrantLevelUpUpgrade() override;

	//직업 고유 강화 후보 풀, 적 BP에서 지정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	TObjectPtr<UUpgradeTableData> classUpgradeTable;

	//보스 유닛 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	bool bIsBoss = false;

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

	//감지·피격 전환 시 근접 합류 파동 반경, 이 안의 같은 진영 캐릭터를 함께 전투로 전환
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection")
	float proximityWaveRadius = 2000.f;

	//전투 행동 결정, 전투 진입 후 자기 턴에 ExecuteTurn 호출
	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UUtilityAIComponent> utilityAI;

	//관측 여부, 한번 관측되면 영구 유지
	bool bVisibleToPlayers = false;

	//아군 시야 스탯(XY 반경) 기준 가시성 갱신, Tick에서 호출, 미관측 상태에서만 판정
	void UpdatePlayerVisibility();
};
