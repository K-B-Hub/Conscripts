// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AilmentBase.generated.h"

class ACharacterBase;

// 모든 상태이상의 공통 기반
// 적용시마다 NewObject로 인스턴스를 생성한다 — OnApply에서 caster 스냅샷을 인스턴스 멤버에 보관하기 위함
UCLASS(Abstract, BlueprintType)
class PW_API UAilmentBase : public UObject
{
	GENERATED_BODY()
public:
	// ─── 지속시간 / 발동 시점 ─────────────────────────────────────
	// 초기 지속 턴수 — 적용 시 remainingTurn으로 복사되는 설정값
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Ailment")
	int32 ailmentTurn = 3;

	// 현재 남은 턴수 — AilmentComponent가 적용시 ailmentTurn으로 초기화, OnTurnEnd마다 차감
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Ailment")
	int32 remainingTurn = 0;

	// 턴 시작시 Execute 작동 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Ailment")
	bool isStart = false;

	// 턴 종료시 Execute 작동 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Ailment")
	bool isEnd = false;

	// 중복 적용 정책
	// true  — 적용시마다 별도 인스턴스로 쌓임 (독립 스택, 만료 각자 처리)
	// false — 같은 클래스가 이미 걸려있으면 턴수만 누적 (단일 인스턴스 갱신)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Ailment")
	bool isStackable = false;

	// 적용 직후 1회 호출 — 파생 클래스가 caster 스탯을 인스턴스 멤버에 스냅샷
	// 이 시점 이후 caster가 GC되어도 Execute는 스냅샷만 사용해 정상 동작
	virtual void OnApply(ACharacterBase* affected, ACharacterBase* caster);

	// 턴 시작/종료 효과 — caster 인자 제거됨
	virtual void Execute(ACharacterBase* affected);
};