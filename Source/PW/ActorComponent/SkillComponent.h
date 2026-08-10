//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"

class USkillBase;
class UActiveSkillBase;
class ASkillRangeIndicator;
class AAttackRangeIndicator;
class ACharacterBase;
class UAnimMontage;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PW_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillComponent();
	
	//소유 캐릭터 지정
	void SetOwner(ACharacterBase* owner);

	//소유 캐릭터 반환
	ACharacterBase* GetOwner();

	//스킬 인스턴스 생성 및 등록
	void AddSkill(TSubclassOf<USkillBase> skillClass);

	//스킬 제거
	void RemoveSkill(USkillBase* Skill);

	//해당 클래스의 스킬을 이미 보유 중인지, 강화 후보 중복 필터용
	bool HasSkillClass(TSubclassOf<USkillBase> skillClass) const;

	//액티브 스킬 목록 반환
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

	//멀티픽 대상 누적
	void AccumulateCurrentTargets();
	const TArray<ACharacterBase*>& GetAccumulatedTargets() const { return accumulatedTargets; }
	void ClearAccumulatedTargets();
	
	//캐릭터의 스탯 변경 시 호출해 스킬들의 계산된 값 변경
	void CalcSkillStats();

	//단일 대상의 피해 예측값 계산
	void RecalculatePending(ACharacterBase* Target);

	//인디케이터 없이 대상들에게 시전, 비용은 1회만 차감, 효과는 pending 커밋 시점에 적용
	void DirectExecute(UActiveSkillBase* Skill, const TArray<ACharacterBase*>& Targets);

	//커밋 대기 중인 스킬 효과 존재 여부, 대기 중엔 컨트롤러가 신규 입력 차단
	bool HasPendingExecution() const { return pendingAction != nullptr; }

	//효과를 pending으로 등록 후 커밋 몽타주 재생, 몽타주 없거나 재생 실패 시 즉시 커밋
	void StartPendingExecution(UActiveSkillBase* Skill, TFunction<void()>&& Action);

	//pending 효과 실행 및 해제, SkillImpact 노티파이 또는 몽타주 종료 폴백이 호출
	void CommitPendingExecution();

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

	//멀티픽 누적 대상 목록
	TArray<ACharacterBase*> accumulatedTargets;

	//커밋 대기 중인 스킬, GC 보호용 (효과 본문은 pendingAction이 보유)
	UPROPERTY()
	TObjectPtr<UActiveSkillBase> pendingSkill = nullptr;

	//pending이 속한 몽타주, 다른 몽타주의 종료 폴백 오발 방지
	UPROPERTY()
	TObjectPtr<UAnimMontage> pendingMontage = nullptr;

	//커밋 시 실행할 효과, 등록 시점에 대상 스냅샷을 약참조로 캡처
	TFunction<void()> pendingAction;

	//노티파이 누락·몽타주 중단 시에도 커밋 보장하는 폴백
	void OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//인디케이터 생성
	void SpawnIndicators(UActiveSkillBase* Skill);
	//인디케이터 제거
	void DestroyIndicators();
};
