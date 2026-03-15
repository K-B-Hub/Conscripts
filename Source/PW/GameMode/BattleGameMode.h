// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleGameMode.generated.h"

class ACharacterBase;
class ABattleController;

UCLASS()
class PW_API ABattleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABattleGameMode();

	// 현재 턴의 유닛이 행동을 마쳤을 때 BattleController에서 호출
	void OnTurnEnd();

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
};