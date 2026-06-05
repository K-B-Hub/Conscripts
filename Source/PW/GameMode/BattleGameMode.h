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

// 플레이어 한 명에 대해 적 AI가 인지하고 있는 "최대 위협" 스냅샷.
// 적 AI 위험도(IncomingDanger) 평가가 이 프로파일을 읽어서 거리·사선·기대 피해를 계산.
// 매 후보 평가마다 PreviewDamage를 호출하지 않음 — 게임 진행 중 관측된 값만 누적.
USTRUCT(BlueprintType)
struct FPlayerThreatProfile
{
	GENERATED_BODY()

	// 기본값 = "약한 적이 한 명쯤 있구나" 정도의 보수적 추정. 첫 관측이 들어오면 RecordPlayerSkillUse가 갱신.
	UPROPERTY(BlueprintReadOnly) float NormalDamage = 10.f;
	UPROPERTY(BlueprintReadOnly) float Accuracy = 50.f;     // %
	UPROPERTY(BlueprintReadOnly) float CritDamage = 20.f;
	UPROPERTY(BlueprintReadOnly) float CritChance = 0.f;    // %
	UPROPERTY(BlueprintReadOnly) float RangeCm = 300.f;     // pickRange 와 같은 단위
};

UCLASS()
class PW_API ABattleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABattleGameMode();

	// 현재 턴의 유닛이 행동을 마쳤을 때 BattleController에서 호출
	void OnTurnEnd();

	// 전역 Conditional 디스패치 — 모든 생존 캐릭터의 conditionType 매칭 패시브 발동
	// RoundStart: 새 라운드 진입 시 (BuildTurnOrder 후, StartCurrentTurn 전)
	// MoveComplete: 캐릭터 이동 완료 시 (이동한 본인 포함 전체 재평가)
	// UnitDeath: 캐릭터 사망 시 (OnCharacterDeath 캐시 정리 후 호출 — 생존자 = allies+enemies)
	//   각 생존 수신자 기준으로 죽은 대상이 같은 팀이면 AllyDeath, 반대 팀이면 EnemyDeath conditionType 발동
	//   (아군 생존자와 적군 생존자에 서로 반대 타입이 디스패치됨)
	void BroadcastRoundStart();
	void BroadcastMoveComplete();
	void BroadcastUnitDeath(ACharacterBase* DeadCharacter);

	// AIController가 감지 평가 시 사용 — 살아있는 아군 목록 조회
	const TArray<TObjectPtr<AAllyCharacterBase>>& GetAllies() const { return allies; }

	// 플레이어 한 명이 스킬을 사용했을 때 그 캐릭터의 위협 프로파일을 갱신.
	// BattleController의 스킬 발동 경로에서 호출 (Skill->BeginUse 직후 권장 — 인디케이터 미리보기 경로와는 구분).
	void RecordPlayerSkillUse(AAllyCharacterBase* Ally, const UActiveSkillBase* Skill);

	// 적 AI가 위험도 계산 시 사용. 미관측 캐릭터면 디폴트 프로파일을 반환 (보수적 초기값).
	const FPlayerThreatProfile& GetPlayerThreatProfile(const AAllyCharacterBase* Ally) const;

protected:
	virtual void BeginPlay() override;

private:
	// 레벨 내 모든 캐릭터를 GetTurnOrder() 내림차순으로 정렬한 배열
	TArray<ACharacterBase*> turnOrder;

	// 현재 진행 중인 턴의 배열 인덱스
	int32 currentTurnIndex = 0;

	// 현재 라운드 번호 (1부터 시작)
	int32 currentRound = 1;

	// 레벨 내 CharacterBase를 수집하고 turnOrder 배열을 구성
	void BuildTurnOrder();

	// turnOrder[currentTurnIndex] 유닛의 턴을 시작
	void StartCurrentTurn();

	// 아군/적군 캐시 배열
	UPROPERTY()
	TArray<TObjectPtr<AAllyCharacterBase>> allies;
	UPROPERTY()
	TArray<TObjectPtr<AEnemyBase>> enemies;

	// 플레이어 별 관측된 최대 위협 프로파일. 키가 없으면 GetPlayerThreatProfile이 디폴트를 반환.
	UPROPERTY()
	TMap<TObjectPtr<AAllyCharacterBase>, FPlayerThreatProfile> playerThreatProfiles;

	// 캐릭터 사망 시 turnOrder/아군/적군 배열 정리
	UFUNCTION()
	void OnCharacterDeath(ACharacterBase* DeadCharacter);

	// 적 사망 시 경험치 분배
	UFUNCTION()
	void OnEnemyDeath(AEnemyBase* DeadEnemy, AAllyCharacterBase* Killer);
};