// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"   // FDamageResult
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

// AI 후보 하나 — (스킬 + 대상 + 시전 위치 + 경로 길이 + 사전계산 피해 + 행동 종류)
// 위치를 후보에 포함하는 이유: 피격 위험·기대 피해 평가가 "어느 위치에서 행동하느냐"에 달려있어
// 위치 결정을 평가 이후로 미루면 평가 시점에 점수를 매길 수 없음 (설계 §5).
USTRUCT(BlueprintType)
struct FAIAction
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UActiveSkillBase> Skill = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ACharacterBase> Target = nullptr;

	// 행동 실행 시 캐스터가 이 위치까지 자동이동 후 스킬 시전. Wait/순수 Move일 땐 의미 다름.
	UPROPERTY(BlueprintReadOnly)
	FVector CastFrom = FVector::ZeroVector;

	// 캐스터 현재 위치 → CastFrom 까지 NavMesh 경로 길이 (cm). 0이면 제자리.
	UPROPERTY(BlueprintReadOnly)
	float PathLengthCm = 0.f;

	UPROPERTY(BlueprintReadOnly)
	FDamageResult Preview;

	UPROPERTY(BlueprintReadOnly)
	EAIActionType Type = EAIActionType::Wait;

	bool IsMove() const { return Type == EAIActionType::Move; }
	bool IsWait() const { return Type == EAIActionType::Wait; }
};