// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleGameMode.generated.h"

class ACharacterBase;
class AAllyCharacterBase;
class AEnemyBase;
class ABattleController;

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

	// 캐릭터 사망 시 turnOrder/아군/적군 배열 정리
	UFUNCTION()
	void OnCharacterDeath(ACharacterBase* DeadCharacter);

	// 적 사망 시 경험치 분배
	UFUNCTION()
	void OnEnemyDeath(AEnemyBase* DeadEnemy, AAllyCharacterBase* Killer);
};