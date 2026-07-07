//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BuffBase.generated.h"

class ACharacterBase;

//모든 버프의 공통 기반
UCLASS(Abstract, BlueprintType)
class PW_API UBuffBase : public UObject
{
	GENERATED_BODY()

public:
	//버프 스탯 변경량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 hp = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 atk = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 speed = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 skill = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 def = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	float movingPoint = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 mentality = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 actionPoint = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 damageReduction = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 damageAmplification = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 penetration = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	float sight = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Combat")
	float accuracy = 0.0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Combat")
	float evasion = 0.0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Combat")
	float critical = 0.0;

	//초기 지속 턴수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Buff")
	int32 buffTurn = 3;

	//현재 남은 턴수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Buff")
	int32 remainingTurn = 0;

	//턴 시작시 Execute 작동 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Buff")
	bool isStart = false;

	//턴 종료시 Execute 작동 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Buff")
	bool isEnd = false;

	//중복 적용 가능 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Buff")
	bool isStackable = false;

	//AI 평가용 기본 가치(턴당, 기대 HP 환산), 스탯 델타로 표현 안 되는 효과(알람 등)의 수동 환산치
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float aiBaseValue = 0.f;

	//적용 직후 1회 호출
	virtual void OnApply(ACharacterBase* affected, ACharacterBase* caster);

	//턴 시작/종료 효과
	virtual void Execute(ACharacterBase* affected);
};
