// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "AllyCharacterBase.generated.h"

class UUpgradeTableData;
class UFixedUpgradeTableData;

//플레이어가 조작하는 아군 캐릭터 베이스
UCLASS()
class PW_API AAllyCharacterBase : public ACharacterBase
{
	GENERATED_BODY()

public:
	virtual bool IsAlly() const override { return true; }

	//직업 고유 강화 후보 풀, 파생 직업 BP에서 지정
	UUpgradeTableData* GetClassUpgradeTable() const { return classUpgradeTable; }

	//직업별 레벨 고정 강화 매핑, 파생 직업 BP에서 지정
	UFixedUpgradeTableData* GetFixedUpgradeTable() const { return fixedUpgradeTable; }

	//대기 중인 강화 개수, 턴 시작 시 BattleController가 소비
	int32 GetPendingUpgradeCount() const { return pendingUpgradeLevels.Num(); }
	void ConsumePendingUpgrade() { if (pendingUpgradeLevels.Num() > 0) pendingUpgradeLevels.RemoveAt(0); }

	//가장 앞 대기 강화가 부여된 레벨, 소비 측이 종류·고정 스킬을 복원하는 데 사용
	int32 PeekPendingUpgradeLevel() const { return pendingUpgradeLevels.Num() > 0 ? pendingUpgradeLevels[0] : 0; }

	//경로 추종 이동 및 이동력 차감 처리
	virtual void Tick(float DeltaTime) override;
	//턴 시작 시 대기 강화가 있으면 OnUpgradeSelectRequested 브로드캐스트
	virtual void InitTurn() override;
	virtual void EndTurn() override;
	virtual void HandleDeath() override;

	//턴 시작 시 대기 중인 강화 선택을 알림, BattleController가 구독해 위젯 표시
	FSimpleMulticastDelegate OnUpgradeSelectRequested;

	//CursorIndicator에서 계산된 경유점 배열을 받아 순서대로 이동
	void MoveAlongPath(const TArray<FVector>& Points);

	//인디케이터가 계산한 포물선 궤적을 3D로 따라 이동(점프), 비용은 호출자가 미리 차감
	void MoveAlongArc(const TArray<FVector>& ArcPoints);

	//이동 중단
	void StopMovement();

	bool IsMoving() const { return bIsMovingToTarget; }

	//점프 궤적 추종 중이면 공중, 애니메이션 상태 판정용
	virtual bool IsAirborne() const override { return bIsJumping; }

	//이동 자연 종료 시 브로드캐스트, 취소/턴종료 시에는 발생하지 않음
	FSimpleMulticastDelegate OnMovementCompleted;

	//목적지 도착 전 감속을 시작할 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float moveDecelRadius = 80.f;

protected:
	//직업 고유 강화 후보 풀, 파생 직업 BP에서 지정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	TObjectPtr<UUpgradeTableData> classUpgradeTable;

	//직업별 레벨 고정 강화 매핑, 파생 직업 BP에서 지정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
	TObjectPtr<UFixedUpgradeTableData> fixedUpgradeTable;

	//레벨업 시 부여 레벨을 큐에 누적, 다음 자기 턴 시작 시 소비. 종류는 소비 시점에 레벨로 분류
	virtual void GrantLevelUpUpgrade() override { pendingUpgradeLevels.Add(level); }

private:
	//대기 중인 강화가 부여된 레벨들(FIFO)
	TArray<int32> pendingUpgradeLevels;

	//NavMesh 경로 경유점 및 현재 인덱스
	TArray<FVector> pathPoints;
	int32 pathPointIndex = 0;

	//정확한 스냅을 위해 최종 목적지 보관
	FVector moveDestination = FVector::ZeroVector;

	bool bIsMovingToTarget = false;

	//이전 프레임 위치, 이동 거리 실시간 차감용
	FVector lastFrameLocation = FVector::ZeroVector;

	//점프 궤적 폴리라인 및 현재 인덱스
	TArray<FVector> arcPoints;
	int32 arcIndex = 0;
	bool bIsJumping = false;

	//점프 궤적 추종 속도(cm/s)
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float jumpSpeed = 800.f;

	//점프 궤적 한 프레임 진행, 완료 시 이동 종료 알림
	void UpdateArcMovement(float DeltaTime);
};