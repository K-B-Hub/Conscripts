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

	// 명중률 자체에 대한 보너스 — 기대 피해에 이미 명중률이 곱해지지만,
	// "고데미지·저명중" vs "중데미지·고명중" 차등을 더 주기 위한 별도 축.
	// HitChance(0~1) × 이 가중치만큼 가산. Attack/Support 후보에 적용.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Threat")
	float weightHitChance = 10.0f;

	// 오버킬 페널티 — max(0, NormalDamage - target.hp) × 이 가중치.
	// 음수로 두면 페널티. hp 3 남은 적에 50뎀 스킬 = 47×weight 페널티로 작은 스킬 선호 유도.
	// 비용 큰 스킬이 정말 필요할 때만 쓰이도록 weightResourceCost와 함께 작동.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Threat")
	float weightOverkillPenalty = -0.5f;

	// 위협이 큰 적을 우선 노리기 — 타겟의 기록된 위협(FPlayerThreatProfile 기대 피해) × 이 가중치.
	// 양수 = "강한 적 먼저 잡자". 위협 무관하게 약한 적 정리 우선이면 0 또는 음수.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Threat")
	float weightTargetThreat = 0.2f;

	// 체력 낮은 적 마무리 — 타겟의 missing HP 비율(1 - hp/maxHp, 0~1) × 이 가중치.
	// 처치 보너스(weightCanKillBonus)는 "확정 처치"에만 발동하지만 본 가중치는
	// hp가 적은 모든 적에 점진 보너스 — 마무리 노리는 일관된 압박.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Threat")
	float weightTargetLowHp = 20.0f;

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

	// 자원 비용 페널티 — (Skill->actionPointCost + battleResourceCost) × 이 가중치.
	// 음수 = "싼 스킬 선호". Attack/Support 후보에만 적용 (Move/Wait는 AP/자원 미소모).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Cost")
	float weightResourceCost = -1.0f;

	// 행동 후 잔여 자원 점수 — (잔여 AP + 잔여 battleResource) × 이 가중치.
	// 양수 = 자원 보존 선호(수비적·"큰 한 방을 위해 모아두기"), 음수 = 자원 소진 선호(공격적·"가진 것 다 쓰자").
	// Move/Wait는 자원 미소모라 현재값 그대로 적용 — 이 가중치가 양수면 행동 안 하기가 유리해지므로 보통 0~약한 부호.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Cost")
	float weightRemainingResource = 0.f;

	// ─── 위험도 축 ────────────────────────────────────────
	// 받을 기대 피해(IncomingDangerExpected, 양수) × 이 가중치 — 페널티로 작용하려면 음수.
	// 디폴트 0 = 기존 성향 행동 무변경. 수비적/저격수 성향에서 -0.5 ~ -2.0 권장.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Risk")
	float weightIncomingDanger = -0.5f;

	// ─── 노이즈 ───────────────────────────────────────────
	// 점수에 ±temperature 균등 노이즈 추가. 0이면 결정론적. "랜덤" 성향 표현용.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weights|Noise")
	float noiseTemperature = 0.0f;
};