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

//플레이어 한 명에 대해 적 AI가 인지하고 있는 최대 위협 스냅샷
USTRUCT(BlueprintType)
struct FPlayerThreatProfile
{
	GENERATED_BODY()

	//기본값은 보수적 추정치
	UPROPERTY(BlueprintReadOnly) float NormalDamage = 10.f;
	UPROPERTY(BlueprintReadOnly) float Accuracy = 50.f;
	UPROPERTY(BlueprintReadOnly) float CritDamage = 20.f;
	UPROPERTY(BlueprintReadOnly) float CritChance = 0.f;
	UPROPERTY(BlueprintReadOnly) float RangeCm = 300.f;
};

UCLASS()
class PW_API ABattleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABattleGameMode();

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

	//플레이어가 스킬을 사용했을 때 그 캐릭터의 위협 프로파일 갱신
	void RecordPlayerSkillUse(AAllyCharacterBase* Ally, const UActiveSkillBase* Skill);

	//적 AI 위험도 계산 시 사용, 미관측 캐릭터면 디폴트 프로파일 반환
	const FPlayerThreatProfile& GetPlayerThreatProfile(const AAllyCharacterBase* Ally) const;

protected:
	virtual void BeginPlay() override;

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

	//플레이어 별 관측된 최대 위협 프로파일, 키가 없으면 디폴트 반환
	UPROPERTY()
	TMap<TObjectPtr<AAllyCharacterBase>, FPlayerThreatProfile> playerThreatProfiles;

	//캐릭터 사망 시 turnOrder/아군/적군 배열 정리
	UFUNCTION()
	void OnCharacterDeath(ACharacterBase* DeadCharacter);

	//적 사망 시 경험치 분배
	UFUNCTION()
	void OnEnemyDeath(AEnemyBase* DeadEnemy, AAllyCharacterBase* Killer);
};