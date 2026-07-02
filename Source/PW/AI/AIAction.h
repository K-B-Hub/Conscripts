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

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ACharacterBase> Target = nullptr;

	//시전 위치
	UPROPERTY(BlueprintReadOnly)
	FVector CastFrom = FVector::ZeroVector;

	//시전 위치까지의 경로 길이
	UPROPERTY(BlueprintReadOnly)
	float PathLengthCm = 0.f;

	UPROPERTY(BlueprintReadOnly)
	FDamageResult Preview;

	//시전 위치의 예상 피격 피해
	UPROPERTY(BlueprintReadOnly)
	float IncomingDangerExpected = 0.f;

	UPROPERTY(BlueprintReadOnly)
	EAIActionType Type = EAIActionType::Wait;

	bool IsMove() const { return Type == EAIActionType::Move; }
	bool IsWait() const { return Type == EAIActionType::Wait; }
};
