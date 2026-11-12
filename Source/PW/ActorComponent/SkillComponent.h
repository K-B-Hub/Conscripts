//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerManager.h"
#include "SkillComponent.generated.h"

class USkillBase;
class UActiveSkillBase;
class ASkillRangeIndicator;
class AAttackRangeIndicator;
class ACharacterBase;
class UAnimMontage;
class AProjectileBase;

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

	//단일 대상의 피해 예측값 계산, 현재 활성 스킬 기준
	void RecalculatePending(ACharacterBase* Target);
	//단일 대상의 피해 예측값 계산, 명시한 스킬 기준 — 커밋 시점 재계산용(currentSkill 해제 후에도 안전)
	void RecalculatePending(UActiveSkillBase* Skill, ACharacterBase* Target);

	//인디케이터 없이 대상들에게 시전, 비용은 1회만 차감, 효과는 pending 커밋 시점에 적용
	//SkillPoint는 AI가 조준한 실제 지점, SkillPoint 기준 밀치기의 기준점으로 기록
	//시전을 시작했으면 true, 결과는 GameMode의 OnActionResolved로 통지된다
	bool DirectExecute(UActiveSkillBase* Skill, const TArray<ACharacterBase*>& Targets, const FVector& SkillPoint);

	//커밋 대기 중인 스킬 효과 존재 여부, 대기 중엔 컨트롤러가 신규 입력 차단
	bool HasPendingExecution() const { return pendingAction != nullptr; }

	//효과를 pending으로 등록 후 커밋 몽타주 재생, 몽타주 없거나 재생 실패 시 즉시 커밋
	//Targets는 연출 카메라가 담을 대상, 행동 시작을 GameMode에 알린다
	void StartPendingExecution(UActiveSkillBase* Skill, const TArray<ACharacterBase*>& Targets, TFunction<void()>&& Action);

	//대상별 판정·효과를 pending으로 등록, 투사체 멀티픽은 대상마다 한 발씩 쏴 도착한 대상부터 적용
	void StartPendingOnTargets(UActiveSkillBase* Skill, const TArray<ACharacterBase*>& Targets);

	//SkillImpact 노티파이 처리, 스킬에 투사체가 있으면 발사해 커밋을 도착 시점으로 미루고 없으면 즉시 커밋
	//멀티픽 몽타주에 노티파이가 둘 이상이면 노티파이마다 한 발
	void OnSkillImpact();

	//pending 효과 실행 및 해제, 투사체 도착·SkillImpact 노티파이·몽타주 종료 폴백이 호출
	void CommitPendingExecution();

	//조준 지점, Self 외 스킬 조준 중이거나 그 스킬의 커밋 대기 중일 때만 true
	bool GetAimPoint(FVector& OutPoint) const;

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

	//커밋까지 유지할 조준 지점, 시전 확정 시 인디케이터에서 기록
	FVector pendingAimPoint = FVector::ZeroVector;
	bool bHasPendingAim = false;

	//비행 중인 투사체, 모두 도착할 때까지 커밋을 보류한다
	UPROPERTY()
	TArray<TObjectPtr<AProjectileBase>> activeProjectiles;

	//효과를 기다리는 대상, 같은 대상을 거듭 고르면 중복 항목
	TArray<TWeakObjectPtr<ACharacterBase>> pendingTargets;

	//아직 쏘지 않은 대상별 투사체와 발사 간격 타이머
	TArray<TWeakObjectPtr<ACharacterBase>> shotQueue;
	FTimerHandle shotTimer;
	//남은 발을 SkillImpact 노티파이가 쏘는지, false면 간격 타이머
	bool bShotsByNotify = false;

	//투사체 포물선 샘플 개수, 조준 중 미리보기 궤적과 같은 밀도
	static constexpr int32 projectileArcSamples = 24;

	//노티파이 누락·몽타주 중단 시에도 커밋 보장하는 폴백
	void OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//시전 이펙트·사운드·몽타주 재생, 연출 카메라 도착을 기다린 뒤 호출
	void PlayPendingPresentation();

	//카메라 도착 대기 타이머
	FTimerHandle presentationTimer;

	//상체 숙이기 정점 커밋 타이머
	FTimerHandle bowImpactTimer;

	//한 대상에 명중 판정과 효과 적용
	void ApplyToTarget(UActiveSkillBase* Skill, ACharacterBase* Target);

	//남은 pendingTargets 전부 적용, StartPendingOnTargets의 커밋 본문
	void ApplyPendingTargets(UActiveSkillBase* Skill);

	//총구 이펙트·사운드, 발마다 재생
	void PlayMuzzleEffects();

	//shotQueue 맨 앞 대상에 한 발, 노티파이 연발이 아니면 간격 뒤 다음 발 예약
	void FireNextShot();

	//몽타주의 SkillImpact 노티파이 개수
	static int32 CountImpactNotifies(const UAnimMontage* Montage);

	//To로 투사체 발사, 발사했으면 true (커밋은 도착 시점으로 미뤄짐)
	//Target을 주면 도착 시 그 대상만 적용, 비우면 도착이 전체 커밋
	bool LaunchProjectile(UActiveSkillBase* Skill, const FVector& To, TWeakObjectPtr<ACharacterBase> Target);

	//투사체 비행 경로 생성, arcApexRatio > 0이면 포물선·그 외는 일자 경로
	TArray<FVector> BuildProjectilePath(const UActiveSkillBase* Skill, const FVector& To) const;

	//투사체 도착 콜백, 대상별이면 그 대상 적용, 마지막 도착이면 커밋
	void OnProjectileArrived(TWeakObjectPtr<AProjectileBase> Projectile, TWeakObjectPtr<ACharacterBase> Target);

	//비행 중 투사체 폐기, 외부 강제 커밋으로 효과가 이미 적용된 경우
	void DiscardActiveProjectiles();

	//인디케이터 생성
	void SpawnIndicators(UActiveSkillBase* Skill);
	//인디케이터 제거
	void DestroyIndicators();
};
