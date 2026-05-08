// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BuffBase.generated.h"

class ACharacterBase;

// 모든 버프의 공통 기반
// 적용시마다 NewObject로 인스턴스를 생성한다 — OnApply에서 caster 스냅샷을 인스턴스 멤버에 보관하기 위함
// 따라서 caster가 사후 사라져도 Execute는 스냅샷만 사용해 안전하게 동작
UCLASS(Abstract, BlueprintType)
class PW_API UBuffBase : public UObject
{
	GENERATED_BODY()

public:
	// ─── 스탯 변경량 ─────────────────────────────────────
	// 버프가 새로 적용 / 사라질 때만 캐릭터에 가감 적용 (매 턴 재계산 X)
	// OnApply에서 caster 스탯에 비례한 값으로 덮어써도 OK — ApplyBuffDelta는 OnApply 이후에 호출됨
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
	// 초기 지속 턴수 — 적용 시 remainingTurn으로 복사되는 설정값
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Buff")
	int32 buffTurn = 3;

	// 현재 남은 턴수 — BuffComponent가 적용시 buffTurn으로 초기화, OnTurnEnd마다 차감
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Buff")
	int32 remainingTurn = 0;

	// 턴 시작시 Execute 작동 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Buff")
	bool isStart = false;

	// 턴 종료시 Execute 작동 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Buff")
	bool isEnd = false;

	// 중복 적용 정책
	// true  — 적용시마다 별도 인스턴스로 쌓임 (독립 스택, 만료/스탯델타 각자 처리)
	// false — 같은 클래스가 이미 걸려있으면 턴수만 누적 (단일 인스턴스 갱신)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Buff")
	bool isStackable = false;

	// ─── 인터페이스 ─────────────────────────────────────
	// 적용 직후 1회 호출 — 파생 클래스가 caster 스탯을 인스턴스 멤버에 스냅샷
	// 이 시점 이후 caster가 GC되어도 Execute는 스냅샷만 사용해 정상 동작
	virtual void OnApply(ACharacterBase* affected, ACharacterBase* caster);

	// 턴 시작/종료 효과 — caster 인자 제거됨
	// 필요한 caster 정보는 OnApply에서 인스턴스 멤버로 스냅샷한 값을 사용
	virtual void Execute(ACharacterBase* affected);
};