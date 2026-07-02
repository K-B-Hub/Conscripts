//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UBehaviorTree;
class UBlackboardData;
class AEnemyBase;

//전투 합류 사유
UENUM(BlueprintType)
enum class EJoinCombatReason : uint8
{
	Detection	UMETA(DisplayName = "감지"),
	Alarm		UMETA(DisplayName = "알람"),
	Proximity	UMETA(DisplayName = "근접 합류"),
};

UCLASS()
class PW_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	//EnemyBase::InitTurn에서 호출
	void OnEnemyTurnStart();

	//턴 종료 처리
	void OnEnemyTurnEnd();

	bool IsInCombat() const { return bIsInCombat; }

	//전투 합류 처리
	void JoinCombat(EJoinCombatReason Reason);

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	//비전투 BT
	UPROPERTY(EditDefaultsOnly, Category = "AI|BehaviorTree")
	TObjectPtr<UBehaviorTree> nonCombatBT;

	//비전투 BT 블랙보드 데이터
	UPROPERTY(EditDefaultsOnly, Category = "AI|BehaviorTree")
	TObjectPtr<UBlackboardData> blackboardData;

	//전투 진입 여부
	bool bIsInCombat = false;

	//블랙보드 키 이름
	static const FName BBKey_TargetActor;
	static const FName BBKey_InCombat;

private:
	//비전투 상태에서 아군 감지 시 전투로 전환
	void EvaluateDetectionAndMaybeJoinCombat();

	//비전투 BT 실행
	void RunCurrentBT();

	//UtilityAI 턴 완료 콜백
	void OnUtilityAITurnComplete();
};
