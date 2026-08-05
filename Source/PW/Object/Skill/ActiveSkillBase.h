//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/SkillBase.h"
#include "Enum/SkillTypes.h"
#include "ActiveSkillBase.generated.h"

class UAnimMontage;
//모든 액티브 스킬의 기반 클래스
UCLASS(Abstract, BlueprintType)
class PW_API UActiveSkillBase : public USkillBase
{
	GENERATED_BODY()

public:
	//스킬 유형
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Targeting")
	ESkillType skillType = ESkillType::Melee;
	//피해 유형
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Targeting")
	EDamageType damageType = EDamageType::Normal;
	//대상 선택 방식
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Targeting")
	ESelectMode selectMode = ESelectMode::SinglePick;
	//선택 가능 팀
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Targeting")
	EPickTeam pickTeam = EPickTeam::EnemyOnly;
	//범위 공격 대상
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Targeting")
	EAreaTarget areaTarget = EAreaTarget::None;
	//범위 형태
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Targeting")
	EAreaForm areaForm = EAreaForm::Circle;
	//범위 파라미터1: Ray=폭, Cone=반지름, Circle=반지름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Range")
	float areaParameter1 = 0.f;
	//범위 파라미터2: Ray=길이, Cone=중심각(도), Circle=미사용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Range")
	float areaParameter2 = 0.f;
	//시전 가능 최대 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Range")
	float pickRange = 0.f;
	//최대 선택 대상 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Targeting")
	int32 pickCount = 1;
	//전투 자원 소모량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cost")
	int32 battleResourceCost = 0;
	//행동력 소모량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cost")
	int32 actionPointCost = 1;
	//스킬의 공격력 계수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Combat")
	float damageRatio = 1.0f;
	//추가 명중률
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Combat")
	float bonusAccuracy = 0.f;
	//추가 치명타율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Combat")
	float bonusCritical = 0.f;
	//스킬 기본 피해량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Combat")
	int32 baseDamage = 0;
	//추가 관통력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Combat")
	int32 bonusPenetration = 0;
	//추가 피해 증폭
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Combat")
	int32 bonusDamageAmplication = 0;
	//스킬 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Animation")
	TObjectPtr<UAnimMontage> skillMontage;
	//몽타주 재생 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Animation")
	float montagePlayRate = 1.0f;
	//포물선 궤적 정점 비율, >0이면 인디케이터가 수평거리×비율 높이의 아치를 계산 (점프·투척 공용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Arc")
	float arcApexRatio = 0.f;
	//아치 이동(점프) 시 수평거리 대비 이동력 소모 배율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Arc")
	float arcMoveCostMultiplier = 1.5f;
	//스킬 사용 가능 여부 검사, 파생에서 추가 조건 가능(알람 1회 제한 등)
	virtual bool CanExecute() const;
	//실행 시 시전자를 궤적을 따라 이동시키는가(점프), 기본은 아님(투척물 등 다른 소비)
	virtual bool UsesArcMovement() const { return false; }
	//투척 스킬 여부, 궤적 표시/비용 분기에 사용
	bool IsThrow() const { return skillType == ESkillType::Throw; }
	//현재 이동력 기준 실제 사용 가능 사거리(cm), 점프는 이동력 예산으로 제한, 그 외는 pickRange
	float GetEffectivePickRange() const;

	void SetCalcedStats();

	float calcDamage;
	float calcAccuracy;
	float calcCritical;
	int32 calcDamageAmplfication;
	int32 calcPenetration;
	//스킬 사용 시 1회 호출
	virtual void BeginUse();
	//적중한 대상마다 호출
	virtual void Execute(const ACharacterBase* target);
};
