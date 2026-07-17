// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleGameMode.generated.h"

class ACharacterBase;
class AAllyCharacterBase;
class AEnemyBase;
class ABattleController;
class UActiveSkillBase;

//캐릭터 한 명에 대해 상대 진영이 인지하고 있는 최대 위협 스냅샷, 진영 무관 관측 기반
USTRUCT(BlueprintType)
struct FThreatProfile
{
	GENERATED_BODY()

	//기본값은 보수적 추정치
	UPROPERTY(BlueprintReadOnly) float NormalDamage = 10.f;
	UPROPERTY(BlueprintReadOnly) float Accuracy = 50.f;
	UPROPERTY(BlueprintReadOnly) float CritDamage = 20.f;
	UPROPERTY(BlueprintReadOnly) float CritChance = 0.f;
	UPROPERTY(BlueprintReadOnly) float RangeCm = 300.f;
};

//턴 순서 배열 재구성 시 통지 (배열, 현재 턴 인덱스)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTurnOrderRebuilt, const TArray<ACharacterBase*>&, int32);
//턴 전환 시 통지 (현재 턴 인덱스)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTurnChanged, int32);

UCLASS()
class PW_API ABattleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABattleGameMode();

	//턴 순서 UI 갱신용 델리게이트
	FOnTurnOrderRebuilt OnTurnOrderRebuilt;
	FOnTurnChanged OnTurnChanged;

	//턴 순서 UI가 생성 시점의 상태를 당겨올 때 사용
	const TArray<ACharacterBase*>& GetTurnOrder() const { return turnOrder; }
	int32 GetCurrentTurnIndex() const { return currentTurnIndex; }

	//현재 턴의 유닛이 행동을 마쳤을 때 BattleController에서 호출
	void OnTurnEnd();

	//새 라운드 진입 시 전역 RoundStart Conditional 디스패치
	void BroadcastRoundStart();
	//캐릭터 이동 완료 시 전역 MoveComplete Conditional 디스패치
	void BroadcastMoveComplete();
	//캐릭터 사망 시 생존자에게 AllyDeath/EnemyDeath Conditional 디스패치
	void BroadcastUnitDeath(ACharacterBase* DeadCharacter);

	//AIController가 감지 평가 시 사용, 살아있는 아군 목록 조회
	const TArray<TObjectPtr<AAllyCharacterBase>>& GetAllies() const { return allies; }

	//AI가 아군(같은 진영) 대상 스킬 후보를 만들 때 사용, 적군(AI 진영) 목록 조회
	const TArray<TObjectPtr<AEnemyBase>>& GetEnemies() const { return enemies; }

	//캐릭터가 스킬을 사용했을 때 위협 프로파일 갱신, 플레이어·AI 공통
	void RecordSkillUse(ACharacterBase* Character, const UActiveSkillBase* Skill);

	//AI 위험도·레버리지 계산 시 사용, 미관측 캐릭터면 디폴트 프로파일 반환
	const FThreatProfile& GetThreatProfile(const ACharacterBase* Character) const;

	//알람 스킬 전역 1회 제한, 한 명이 쓰면 모든 보유자가 재사용 불가
	bool IsAlarmUsed() const { return bAlarmUsed; }
	void MarkAlarmUsed() { bAlarmUsed = true; }

	//스트레스 한계 도달 시 호출, 20% 긍정 / 80% 부정 풀에서 랜덤 이벤트를 대상에게 적용
	void ApplyStressEvent(ACharacterBase* Target);

protected:
	virtual void BeginPlay() override;

	//스트레스 이벤트 풀, 버프/상태이상/패시브 클래스를 담고 적용 시 계열별로 분기
	UPROPERTY(EditDefaultsOnly, Category = "Stress")
	TArray<TSubclassOf<UObject>> positiveStressEvents;
	UPROPERTY(EditDefaultsOnly, Category = "Stress")
	TArray<TSubclassOf<UObject>> negativeStressEvents;

private:
	//레벨 내 모든 캐릭터를 GetTurnOrder() 내림차순으로 정렬한 배열
	TArray<ACharacterBase*> turnOrder;

	//현재 진행 중인 턴의 배열 인덱스
	int32 currentTurnIndex = 0;

	//현재 라운드 번호
	int32 currentRound = 1;

	//레벨 내 CharacterBase를 수집하고 turnOrder 배열을 구성
	void BuildTurnOrder();

	//turnOrder[currentTurnIndex] 유닛의 턴을 시작
	void StartCurrentTurn();

	//아군/적군 캐시 배열
	UPROPERTY()
	TArray<TObjectPtr<AAllyCharacterBase>> allies;
	UPROPERTY()
	TArray<TObjectPtr<AEnemyBase>> enemies;

	//캐릭터 별 관측된 최대 위협 프로파일, 키가 없으면 디폴트 반환
	UPROPERTY()
	TMap<TObjectPtr<ACharacterBase>, FThreatProfile> threatProfiles;

	//알람 스킬 사용 여부
	bool bAlarmUsed = false;

	//캐릭터 사망 시 turnOrder/아군/적군 배열 정리
	UFUNCTION()
	void OnCharacterDeath(ACharacterBase* DeadCharacter);

	//적 사망 시 경험치 분배
	UFUNCTION()
	void OnEnemyDeath(AEnemyBase* DeadEnemy, AAllyCharacterBase* Killer);
};