//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"   //FDamageResult
#include "AIAction.generated.h"

class UActiveSkillBase;
class ACharacterBase;

UENUM(BlueprintType)
enum class EAIActionType : uint8
{
	Attack	UMETA(DisplayName = "공격"),
	Support	UMETA(DisplayName = "지원"),
	Move	UMETA(DisplayName = "이동"),
	Wait	UMETA(DisplayName = "대기"),
};

//AI 행동 후보
USTRUCT(BlueprintType)
struct FAIAction
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UActiveSkillBase> Skill = nullptr;

	//영향을 받는 모든 대상, [0]은 대표 대상(회전·처치 보너스·디버그 기준)
	//단일 대상 스킬도 원소 1개로 통일해 분기 없이 처리
	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<ACharacterBase>> Targets;

	//시전 위치
	UPROPERTY(BlueprintReadOnly)
	FVector CastFrom = FVector::ZeroVector;

	//시전 위치까지의 경로 길이
	UPROPERTY(BlueprintReadOnly)
	float PathLengthCm = 0.f;

	//최근접 플레이어와의 거리 감소량, 이동 후보 전용
	UPROPERTY(BlueprintReadOnly)
	float ApproachDeltaCm = 0.f;

	//대표 대상의 피해 예측
	UPROPERTY(BlueprintReadOnly)
	FDamageResult Preview;

	//적군(상대 진영) 대상 기대 피해 합산
	UPROPERTY(BlueprintReadOnly)
	float EnemyExpectedDamage = 0.f;

	//아군(같은 진영) 오사 기대 피해 합산, 점수 페널티용
	UPROPERTY(BlueprintReadOnly)
	float AllyExpectedDamage = 0.f;

	//적군 대상들의 평균 명중률(0~100)
	UPROPERTY(BlueprintReadOnly)
	float AvgEnemyHitChance = 0.f;

	//기대 처치 수, 적군 대상별 (처치 가능 ? 명중확률 : 0) 합산
	UPROPERTY(BlueprintReadOnly)
	float ExpectedKills = 0.f;

	//기대 치명 처치 수, 적군 대상별 (치명 처치 가능 ? 명중*치명확률 : 0) 합산
	UPROPERTY(BlueprintReadOnly)
	float ExpectedCritKills = 0.f;

	//적군 대상들의 오버킬 피해 합산
	UPROPERTY(BlueprintReadOnly)
	float OverkillTotal = 0.f;

	//아군 대상들의 유효 회복량 합산, 오버힐(최대 체력 초과분) 제외
	UPROPERTY(BlueprintReadOnly)
	float HealExpectedTotal = 0.f;

	//회복 대상 중 체력 50% 미만 수
	UPROPERTY(BlueprintReadOnly)
	float HealLowHpCount = 0.f;

	//회복 대상 중 체력 20% 미만 수
	UPROPERTY(BlueprintReadOnly)
	float HealExtremeLowHpCount = 0.f;

	//버프 기대 가치 합산, 기대 HP 환산·팀부호 반영
	UPROPERTY(BlueprintReadOnly)
	float BuffValueTotal = 0.f;

	//상태이상 기대 가치 합산, 기대 HP 환산·팀부호 반영
	UPROPERTY(BlueprintReadOnly)
	float AilmentValueTotal = 0.f;

	//시전 위치의 예상 피격 피해
	UPROPERTY(BlueprintReadOnly)
	float IncomingDangerExpected = 0.f;

	UPROPERTY(BlueprintReadOnly)
	EAIActionType Type = EAIActionType::Wait;

	bool IsMove() const { return Type == EAIActionType::Move; }
	bool IsWait() const { return Type == EAIActionType::Wait; }

	//대표 대상, 대상이 없으면 nullptr
	ACharacterBase* GetPrimaryTarget() const { return Targets.Num() > 0 ? Targets[0] : nullptr; }
};
