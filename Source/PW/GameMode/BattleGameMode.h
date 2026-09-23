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
class ATerrainBase;
class ADeploymentZone;
class UMissionData;

//전투 맵의 진행 단계, Deploy 동안에는 턴이 시작되지 않는다
UENUM()
enum class EBattlePhase : uint8
{
	Deploy,		//로스터를 배치 구획에 놓는 중
	Battle,		//턴제 전투 진행 중
	Result		//승패가 확정되어 턴 진행이 멈춘 상태
};

//전투 종료 사유
UENUM()
enum class EBattleResult : uint8
{
	Victory,	//임무 달성
	Defeat		//아군 전멸
};

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
//Deploy 페이즈 진입 시 통지, 컨트롤러가 배치 위젯을 띄운다
DECLARE_MULTICAST_DELEGATE(FOnDeployPhaseStarted);
//전투 종료 시 통지, 컨트롤러가 결과 위젯을 띄운다
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleFinished, EBattleResult);

UCLASS()
class PW_API ABattleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABattleGameMode();

	//턴 순서 UI 갱신용 델리게이트
	FOnTurnOrderRebuilt OnTurnOrderRebuilt;
	FOnTurnChanged OnTurnChanged;

	//Deploy 페이즈 진입 통지
	FOnDeployPhaseStarted OnDeployPhaseStarted;

	//전투 종료 통지
	FOnBattleFinished OnBattleFinished;

	EBattlePhase GetPhase() const { return phase; }

	//로스터의 해당 인원을 지점에 배치, 이미 배치되어 있으면 위치만 옮긴다
	//구획 밖이거나 Deploy 페이즈가 아니면 false
	bool DeployAt(int32 rosterIndex, const FVector& location);

	//해당 인원이 이미 배치되었는지, 배치 위젯의 표시용
	bool IsDeployed(int32 rosterIndex) const;

	//로스터 전원이 배치되었는지
	bool IsDeploymentComplete() const;

	//배치 확정, 전원 배치된 경우에만 전투를 시작한다
	void ConfirmDeployment();

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

	//전투 중 아군 합류, 증원 강화가 호출한다
	//caller 옆에 레벨 1로 세운 뒤 평균 레벨까지 올리고, 현재 턴 바로 다음 순서에 끼워 넣는다
	//쌓인 대기 강화는 신병의 턴 시작에서 기존 경로대로 소비된다
	AAllyCharacterBase* JoinReinforcement(TSubclassOf<AAllyCharacterBase> jobClass, const AAllyCharacterBase* caller);

	//생존 아군의 평균 레벨, 최소 1
	int32 GetAverageAllyLevel() const;

	//지형 자기 등록, AI가 경로마다 액터를 순회하지 않도록 캐시
	void RegisterTerrain(ATerrainBase* Terrain);
	void UnregisterTerrain(ATerrainBase* Terrain);
	const TArray<TObjectPtr<ATerrainBase>>& GetTerrains() const { return terrains; }

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

private:
	//현재 진행 단계
	EBattlePhase phase = EBattlePhase::Deploy;

	//배치 구획 수집 및 배치 위젯 표시
	void StartDeployPhase();

	//로스터 스폰 이후의 전투 개시, 기존 BeginPlay 초기화가 통째로 여기로 옮겨졌다
	void StartBattlePhase();

	//승패 판정, 사망 처리와 턴 종료 양쪽에서 호출된다
	//사망 처리 도중 레벨 전환이 시작되지 않도록 판정만 하고 종료는 다음 틱으로 미룬다
	void EvaluateMission();

	//전투 종료 확정, 생존자 스냅샷을 회수하고 결과를 통지
	void FinishBattle(EBattleResult result);

	//이번 스테이지의 승리 조건, 시퀀스에서 가져온다
	const UMissionData* GetCurrentMission() const;

	//지점이 배치 구획 안인지
	bool IsInsideDeploymentZone(const FVector& point) const;

	//레벨에 놓인 배치 구획
	UPROPERTY()
	TArray<TObjectPtr<ADeploymentZone>> deploymentZones;

	//로스터 인덱스 → 배치된 캐릭터
	UPROPERTY()
	TMap<int32, TObjectPtr<AAllyCharacterBase>> deployedAllies;

	//레벨 내 모든 캐릭터를 GetTurnOrder() 내림차순으로 정렬한 배열
	TArray<ACharacterBase*> turnOrder;

	//현재 진행 중인 턴의 배열 인덱스
	int32 currentTurnIndex = 0;

	//현재 라운드 번호
	int32 currentRound = 1;

	//레벨 내 CharacterBase를 수집하고 turnOrder 배열을 구성
	void BuildTurnOrder();

	//caller 옆에서 NavMesh 위 빈 지점을 찾는다, 못 찾으면 false
	bool FindReinforcementSpot(const AAllyCharacterBase* caller, FVector& outLocation) const;

	//해당 지점 주변 clearance 안에 다른 캐릭터가 없는지, 높이는 무시하고 평면 거리로 본다
	bool IsSpotClear(const FVector& location, float clearance) const;

	//적 초기 레벨 스케일링, 아군 평균 레벨 +2까지 레벨업하며 랜덤 강화 자동 습득
	void ApplyEnemyLevelScaling();

	//turnOrder[currentTurnIndex] 유닛의 턴을 시작
	void StartCurrentTurn();

	//아군/적군 캐시 배열
	UPROPERTY()
	TArray<TObjectPtr<AAllyCharacterBase>> allies;
	UPROPERTY()
	TArray<TObjectPtr<AEnemyBase>> enemies;

	//레벨 내 지형 캐시, 각 지형이 BeginPlay에서 자기 등록
	UPROPERTY()
	TArray<TObjectPtr<ATerrainBase>> terrains;

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