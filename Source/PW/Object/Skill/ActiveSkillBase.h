// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/SkillBase.h"
#include "Enum/SkillTypes.h"
#include "ActiveSkillBase.generated.h"

class UAnimMontage;

// 모든 액티브 스킬의 기반 클래스
// 타겟팅 플래그, 범위, 전투 스탯 수정치를 보유하며 Execute()를 통해 실행
UCLASS(Abstract, BlueprintType)
class PW_API UActiveSkillBase : public USkillBase
{
	GENERATED_BODY()

public:
	// ─── 스킬 분류 플래그 ─────────────────────────────────────

	// 스킬 유형 (근접/원거리/투척/버프)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Targeting")
	ESkillType skillType = ESkillType::Melee;

	// 피해 유형 (명중 기반 / 광역 확정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Targeting")
	EDamageType damageType = EDamageType::Normal;

	// 대상 선택 방식 (본인/단일지정/지점지정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Targeting")
	ESelectMode selectMode = ESelectMode::SinglePick;

	// 선택 가능 팀 (적/아군/전체) — selectMode가 SinglePick, GroundPoint일 때 유효
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Targeting")
	EPickTeam pickTeam = EPickTeam::EnemyOnly;

	// 범위 공격 대상 (없음/적/아군/전체)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Targeting")
	EAreaTarget areaTarget = EAreaTarget::None;

	// 범위 형태 — areaTarget이 None이 아닐 때만 유효
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Targeting")
	EAreaForm areaForm = EAreaForm::Circle;

	// ─── 범위 파라미터 ────────────────────────────────────────

	// 범위 파라미터1: Ray=폭, Cone=반지름, Circle=반지름 (cm 단위)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Range")
	float areaParameter1 = 0.f;

	// 범위 파라미터2: Ray=길이, Cone=중심각(도), Circle=미사용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Range")
	float areaParameter2 = 0.f;

	// 시전 가능 최대 거리 (cm 단위, 0=무제한)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Range")
	float pickRange = 0.f;

	// ─── 다중 선택 ────────────────────────────────────────────

	// SinglePick 모드에서 지정 가능한 최대 대상 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Targeting")
	int32 pickCount = 1;

	// ─── 자원 비용 ────────────────────────────────────────────

	// 전투 자원 소모량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cost")
	int32 battleResourceCost = 0;
	
	// 스킬 사용 시 소모하는 행동력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cost")
	int32 actionPointCost = 1;
	
	// ─── AttackIndicator에 전달되는 전투 스탯 수정치 ──────────
	//스킬의 공격력 계수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Combat")
	float damageRatio = 1.0f;
	
	// 추가 명중률
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Combat")
	float bonusAccuracy = 0.f;

	// 추가 치명타율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Combat")
	float bonusCritical = 0.f;

	// 스킬 기본 피해량 (캐릭터 atk에 더해지는 고정값)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Combat")
	int32 baseDamage = 0;

	// 추가 관통력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Combat")
	int32 bonusPenetration = 0;

	// 추가 피해 증폭
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Combat")
	int32 bonusDamageAmplication = 0;

	// ─── 애니메이션 ──────────────────────────────────────────

	// 스킬 사용 시 재생할 애님 몽타주 (없으면 재생 생략)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Animation")
	TObjectPtr<UAnimMontage> skillMontage;

	// 몽타주 재생 속도 (1.0 = 원본 속도, 낮을수록 느림)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Animation")
	float montagePlayRate = 1.0f;

	// ─── 인터페이스 ──────────────────────────────────────────

	// 스킬 사용 가능 여부 검사 (AP, 전투 자원)
	bool CanExecute() const;

	void SetCalcedStats();

	float calcDamage;
	float calcAccuracy;
	float calcCritical;
	int32 calcDamageAmplfication;
	int32 calcPenetration;

	// 스킬 사용 시 1회 호출 — 자원 차감 + 몽타주 재생 (적중 여부 무관)
	virtual void BeginUse();

	// 적중한 타겟마다 호출 — 버프 적용 등 hit-gated 효과
	virtual void Execute(const ACharacterBase* target);
};
