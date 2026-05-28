// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AIPersonalityData.generated.h"

// 적 AI 성향별 스코어링 가중치 — 데이터에셋으로 관리해 BP에서 적별로 다른 인스턴스 할당.
// 동일 후보 평가 파이프라인에 가중치 벡터만 교체 (설계 §8).
// 노이즈는 "랜덤" 성향을 별도 후보가 아닌 점수 흔들기로 흡수.
UCLASS(BlueprintType)
class PW_API UAIPersonalityData : public UDataAsset
{
	GENERATED_BODY()

public:
	// 디자이너 식별용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	FName personalityName = TEXT("Normal");

	// ─── 위협도(처치) 축 ──────────────────────────────────
	// 기대 피해 1당 점수 — 공격적 성향에서 크게
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Threat")
	float weightExpectedDamage = 1.0f;

	// 확정 처치(NormalDamage >= hp) 가능 시 보너스 (HitChance로 가중)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Threat")
	float weightCanKillBonus = 50.0f;

	// 운빨 처치(치명타 시에만 처치) 가능 시 보너스 (HitChance × CritChance로 가중)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Threat")
	float weightCanCritKillBonus = 15.0f;

	// ─── 행동 비용 축 ────────────────────────────────────
	// 이동 거리 cm당 점수 — 음수가 페널티. 수비적 성향에서 크게 (이동 최소화)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Cost")
	float weightDistancePenalty = -0.001f;

	// 대기 후보의 절대 점수 — 음수면 다른 후보 없을 때만 선택됨
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Cost")
	float weightWait = -10.0f;

	// 순수 이동(접근) cm당 보너스 — 공격적 성향에서 크게 (적극 전진)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Cost")
	float weightMoveAdvance = 0.005f;

	// ─── 노이즈 ───────────────────────────────────────────
	// 점수에 ±temperature 균등 노이즈 추가. 0이면 결정론적. "랜덤" 성향 표현용.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Noise")
	float noiseTemperature = 0.0f;
};