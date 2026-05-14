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

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PW_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillComponent();
	
	void SetOwner(ACharacterBase* owner);
	ACharacterBase* GetOwner();

	//스킬 클래스로 인스턴스 생성 후 등록 — AddPassive와 동일 패턴
	void AddSkill(TSubclassOf<USkillBase> skillClass);

	//스킬 제거
	void RemoveSkill(USkillBase* Skill);

	TArray<UActiveSkillBase*> GetActiveSkills() const;
	const TArray<TObjectPtr<USkillBase>>& GetAllSkills() const { return skills; }
	
	//스킬 활성화, 인디케이터 생성으로 연결
	void ActivateSkill(UActiveSkillBase* Skill);

	//스킬 비활성화, 인디케이터 제거로 연결
	void DeactivateSkill();

	UActiveSkillBase* GetCurrentSkill() const { return currentSkill; }
	bool IsSkillActive() const { return currentSkill != nullptr; }

	AAttackRangeIndicator* GetAttackRangeIndicator() const { return attackRangeIndicator; }

	//현재 스킬 영향 범위 내 대상 목록
	const TArray<ACharacterBase*>& GetCurrentTargets() const { return currentTargets; }
	void AddTarget(ACharacterBase* Target);
	void RemoveTarget(ACharacterBase* Target);
	void ClearCurrentTargets();

	// 멀티픽: 현재 오버랩 대상을 누적 배열에 스냅샷 저장
	void AccumulateCurrentTargets();
	const TArray<ACharacterBase*>& GetAccumulatedTargets() const { return accumulatedTargets; }
	void ClearAccumulatedTargets();
	
	//캐릭터의 스탯 변경 시 호출해 스킬들의 계산된 값 변경
	void CalcSkillStats();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	ACharacterBase* ownerCharacter = nullptr;
	//보유 스킬 목록
	UPROPERTY()
	TArray<TObjectPtr<USkillBase>> skills;

	//현재 사용중인 스킬
	UPROPERTY()
	TObjectPtr<UActiveSkillBase> currentSkill = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|Indicator")
	TSubclassOf<ASkillRangeIndicator> skillRangeIndicatorClass;
	UPROPERTY(EditDefaultsOnly, Category = "Skill|Indicator")
	TSubclassOf<AAttackRangeIndicator> attackRangeIndicatorClass;

	//스폰된 인디케이터 인스턴스
	UPROPERTY()
	TObjectPtr<ASkillRangeIndicator> skillRangeIndicator = nullptr;
	UPROPERTY()
	TObjectPtr<AAttackRangeIndicator> attackRangeIndicator = nullptr;

	//스킬 영향 범위 내 대상 목록
	TArray<ACharacterBase*> currentTargets;

	// 멀티픽용 누적 대상 목록 (각 pick 시점의 오버랩 대상 스냅샷)
	TArray<ACharacterBase*> accumulatedTargets;

	//인디케이터 생성
	void SpawnIndicators(UActiveSkillBase* Skill);
	//인디케이터 제거
	void DestroyIndicators();
};