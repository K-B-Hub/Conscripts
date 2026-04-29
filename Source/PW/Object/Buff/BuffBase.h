// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BuffBase.generated.h"

class ACharacterBase;

// 모든 버프의 공통 기반 — 클래스당 CDO 한 개를 공유 템플릿으로 사용
// 런타임 상태(남은 턴수, 시전자)는 BuffComponent 측 FActiveBuff에 보관
// 따라서 이 클래스는 stateless 데이터 + 가상 Execute 만 보유
UCLASS(Abstract, BlueprintType)
class PW_API UBuffBase : public UObject
{
	GENERATED_BODY()

public:
	// ─── 스탯 변경량 ─────────────────────────────────────
	// 버프가 새로 적용 / 사라질 때만 캐릭터에 가감 적용 (매 턴 재계산 X)
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

	// ─── 지속시간 / 발동 시점 ─────────────────────────────────────
	// 초기 지속 턴수 — BuffComponent가 적용 시 FActiveBuff.remainingTurn으로 복사
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Buff")
	int32 buffTurn = 3;

	// 턴 시작시 Execute 작동 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Buff")
	bool isStart = false;

	// 턴 종료시 Execute 작동 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Buff")
	bool isEnd = false;

	// ─── 인터페이스 ─────────────────────────────────────
	// 특수효과 — 파생 클래스에서 도트, 회복, 상태이상 트리거 등 구현
	// affected: 버프가 붙어있는 캐릭터 (피영향자)
	// caster: 버프를 건 캐릭터 (이미 죽었을 수 있음 → null 분기 필요)
	virtual void Execute(ACharacterBase* affected, ACharacterBase* caster);
};