// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "Run/AllyRunState.h"
#include "AllyCharacterBase.generated.h"

class UUpgradeTableData;
class UFixedUpgradeTableData;
class UWidgetComponent;

//플레이어가 조작하는 아군 캐릭터 베이스
UCLASS()
class PW_API AAllyCharacterBase : public ACharacterBase
{
	GENERATED_BODY()

public:
	AAllyCharacterBase();

	virtual bool IsAlly() const override { return true; }

	//야영지 말풍선을 켜고 끈다, 대사가 비어 있으면 표시하지 않는다
	void ShowCampLine(const FText& line);
	void HideCampLine();

	//야영지 정비 회복
	//ReceiveDamage(음수)는 추가 스트레스 감소와 Damaged 패시브를 함께 일으켜
	//정비 선택지의 설계 수치를 넘기므로 부수효과 없는 전용 경로를 쓴다
	void RestAtCamp(int32 healAmount, int32 stressRelief);

	//편성·출격 화면에 표시할 직업 이름, 파생 직업 BP에서 지정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Job")
	FText jobName;

	//개체 이름, 로스터 항목마다 다르며 복원 시 주입된다
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Job")
	FString displayName;

	//야영지 대기 애니메이션 후보, 파생 직업 BP에서 지정
	//몽타주는 스켈레톤에 묶여 직업 간 공유가 불가능하므로 자리가 아니라 캐릭터가 들고 있는다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camp")
	TArray<TObjectPtr<UAnimMontage>> campIdleMontages;

	//야영지 말풍선, 머리 위에 표시되며 야영지 밖에서는 꺼져 있다
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camp")
	TObjectPtr<UWidgetComponent> campDialogueComponent;

	//후보 중 하나를 무작위로 재생, 비어 있으면 아무것도 하지 않는다
	void PlayRandomCampIdle();

	//현재 상태를 스냅샷으로 추출, CDO에 대해 호출하면 직업 기본 스탯이 나온다
	FAllyRunState CaptureRunState() const;

	//스냅샷 복원, 강화 재등록 → 베이스 스탯 대입 → 파생 재계산 순서를 지킨다
	void RestoreRunState(const FAllyRunState& state);

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

	//걷기 이동 상태 정리 후 공용 아치 이동(점프), 비용은 호출자가 미리 차감
	virtual void MoveAlongArc(const TArray<FVector>& ArcPoints, bool bForced = false) override;

	//이동 중단
	void StopMovement();

	bool IsMoving() const { return bIsMovingToTarget; }

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

	//아치 이동(점프) 자연 완료 시 이동 완료 통지
	virtual void NotifyArcMoveCompleted() override;

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
};