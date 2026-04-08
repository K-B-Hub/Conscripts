// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"

class USkillBase;
class UActiveSkillBase;
class ASkillRangeIndicator;
class AAttackRangeIndicator;
class ACharacterBase;

// 캐릭터의 스킬 관리 및 사용 주체
// 외부에서 생성된 스킬 오브젝트를 받아 보관하고, 활성화 시 인디케이터 생성/제거
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PW_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillComponent();
	
	void SetOwner(ACharacterBase* owner);
	ACharacterBase* GetOwner();

	// ─── 스킬 추가/제거 ──────────────────────────────────────

	// 외부에서 생성된 스킬 오브젝트를 등록
	void AddSkill(USkillBase* Skill);

	// 스킬 제거
	void RemoveSkill(USkillBase* Skill);

	// ─── 스킬 조회 ───────────────────────────────────────────

	TArray<UActiveSkillBase*> GetActiveSkills() const;
	const TArray<TObjectPtr<USkillBase>>& GetAllSkills() const { return skills; }

	// ─── 스킬 활성화/비활성화 ────────────────────────────────

	// 스킬 활성화 — 인디케이터 생성, 이전 스킬은 자동 비활성화
	void ActivateSkill(UActiveSkillBase* Skill);

	// 스킬 비활성화 — 인디케이터 제거
	void DeactivateSkill();

	UActiveSkillBase* GetCurrentSkill() const { return currentSkill; }
	bool IsSkillActive() const { return currentSkill != nullptr; }

	// 현재 스킬 영향 범위 내 대상 목록
	const TArray<ACharacterBase*>& GetCurrentTargets() const { return currentTargets; }
	void AddTarget(ACharacterBase* Target);
	void RemoveTarget(ACharacterBase* Target);
	void ClearCurrentTargets();
	
	//캐릭터의 스탯 변경 시 호출해 스킬들의 계산된 값 변경
	void CalcSkillStats();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	ACharacterBase* ownerCharacter = nullptr;
	// 보유 스킬 목록
	UPROPERTY()
	TArray<TObjectPtr<USkillBase>> skills;

	// 현재 활성화된 스킬
	UPROPERTY()
	TObjectPtr<UActiveSkillBase> currentSkill = nullptr;

	// 인디케이터 클래스 (BP에서 지정)
	UPROPERTY(EditDefaultsOnly, Category = "Skill|Indicator")
	TSubclassOf<ASkillRangeIndicator> skillRangeIndicatorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|Indicator")
	TSubclassOf<AAttackRangeIndicator> attackRangeIndicatorClass;

	// 스폰된 인디케이터 인스턴스
	UPROPERTY()
	TObjectPtr<ASkillRangeIndicator> skillRangeIndicator = nullptr;

	UPROPERTY()
	TObjectPtr<AAttackRangeIndicator> attackRangeIndicator = nullptr;

	// 스킬 영향 범위 내 대상 목록
	TArray<ACharacterBase*> currentTargets;

	void SpawnIndicators(UActiveSkillBase* Skill);
	void DestroyIndicators();
};