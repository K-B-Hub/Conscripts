//Fill out your copyright notice in the Description page of Project Settings.

#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/SkillBase.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "Actors/SkillRangeIndicator.h"
#include "Actors/AttackRangeIndicator.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/PassiveSkillComponent.h"
#include "GameMode/BattleGameMode.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USkillComponent::SetOwner(ACharacterBase* owner)
{
	if (owner->IsValidLowLevel())
	{
		ownerCharacter = owner;
	}
}

ACharacterBase* USkillComponent::GetOwner()
{
		return ownerCharacter;
}

void USkillComponent::BeginPlay()
{
	Super::BeginPlay();

	//CDO에서 실제 객체로 재등록
	ownerCharacter = Cast<ACharacterBase>(UActorComponent::GetOwner());
}

void USkillComponent::AddSkill(TSubclassOf<USkillBase> skillClass)
{
	if (!skillClass || !ownerCharacter) return;

	USkillBase* instance = NewObject<USkillBase>(this, skillClass);
	if (!instance) return;

	instance->SetOwner(ownerCharacter);
	skills.Add(instance);

	if (UActiveSkillBase* ActiveSkill = Cast<UActiveSkillBase>(instance))
	{
		ActiveSkill->SetCalcedStats();
	}
}

bool USkillComponent::HasSkillClass(TSubclassOf<USkillBase> skillClass) const
{
	if (!skillClass) return false;

	for (const USkillBase* skill : skills)
	{
		if (skill && skill->GetClass() == skillClass)
		{
			return true;
		}
	}
	return false;
}

void USkillComponent::RemoveSkill(USkillBase* Skill)
{
	if (!Skill) return;

	if (currentSkill == Skill)
	{
		DeactivateSkill();
	}
	skills.Remove(Skill);
}

TArray<UActiveSkillBase*> USkillComponent::GetActiveSkills() const
{
	TArray<UActiveSkillBase*> ActiveSkills;
	for (USkillBase* Skill : skills)
	{
		if (UActiveSkillBase* Active = Cast<UActiveSkillBase>(Skill))
		{
			ActiveSkills.Add(Active);
		}
	}
	return ActiveSkills;
}

void USkillComponent::ActivateSkill(UActiveSkillBase* Skill)
{
	if (!Skill || !Skill->CanExecute()) return;

	//이전 스킬 커밋 대기 중 신규 활성화 차단
	if (HasPendingExecution()) return;

	if (currentSkill)
	{
		DeactivateSkill();
	}

	currentSkill = Skill;
	SpawnIndicators(Skill);

	UE_LOG(LogTemp, Log, TEXT("[SkillComponent] 스킬 활성화: %s"), *Skill->skillName.ToString());
}

void USkillComponent::DeactivateSkill()
{
	if (!currentSkill) return;

	UE_LOG(LogTemp, Log, TEXT("[SkillComponent] 스킬 비활성화: %s"), *currentSkill->skillName.ToString());

	//인디케이터와 대상 목록 정리
	DestroyIndicators();
	ClearCurrentTargets();
	ClearAccumulatedTargets();
	currentSkill = nullptr;

	//인디케이터가 자동이동 예상으로 켜둔 투기적 이동 상태를 실제 이동 여부로 복구
	//최대치 비교는 달리기 보너스·이동력 버프에 흔들리므로 소모 플래그를 사용
	if (ownerCharacter)
	{
		ownerCharacter->OnMoveStateChanged(ownerCharacter->HasConsumedMovingPoint());
	}
}

void USkillComponent::AddTarget(ACharacterBase* Target)
{
	if (Target)
	{
		currentTargets.AddUnique(Target);
	}
}

void USkillComponent::RemoveTarget(ACharacterBase* Target)
{
	if (Target)
	{
		currentTargets.Remove(Target);
	}
}

void USkillComponent::ClearCurrentTargets()
{
	currentTargets.Empty();
}

void USkillComponent::AccumulateCurrentTargets()
{
	accumulatedTargets.Append(currentTargets);
}

void USkillComponent::ClearAccumulatedTargets()
{
	accumulatedTargets.Empty();
}

void USkillComponent::CalcSkillStats()
{
	for (USkillBase* Skill : skills)
	{
		if (UActiveSkillBase* Active = Cast<UActiveSkillBase>(Skill))
		{
			Active->SetCalcedStats();
		}
	}
}

void USkillComponent::RecalculatePending(ACharacterBase* Target)
{
	RecalculatePending(currentSkill, Target);
}

void USkillComponent::RecalculatePending(UActiveSkillBase* Skill, ACharacterBase* Target)
{
	if (!Target || !Skill || !ownerCharacter) return;

	//스킬 계산값을 로컬 값으로 복사
	float dmg  = Skill->calcDamage;
	int32 amp  = Skill->calcDamageAmplfication;
	int32 pen  = Skill->calcPenetration;
	float acc  = Skill->calcAccuracy;
	float crit = Skill->calcCritical;

	//공격자 패시브 보너스 반영
	if (UPassiveSkillComponent* PSC = ownerCharacter->GetPassiveSkillComponent())
	{
		PSC->DispatchBeforeDamageCalc(Target, Skill->skillType, Skill->damageType,
			dmg, amp, pen, acc, crit);
	}

	Target->CalculateDamage(
		dmg,
		acc,
		crit,
		amp,
		pen,
		Skill->skillType,
		ownerCharacter
	);
}

void USkillComponent::DirectExecute(UActiveSkillBase* Skill, const TArray<ACharacterBase*>& Targets, const FVector& SkillPoint)
{
	if (!Skill || Targets.Num() == 0 || !ownerCharacter) return;
	if (!Skill->CanExecute()) return;

	//AI가 조준한 실제 지점을 밀치기 기준점으로 기록, Caster 기준 스킬은 Execute가 시전자 위치를 직접 사용
	Skill->skillPointLocation = SkillPoint;

	//자원 차감은 1회만, 확정 시점 즉시
	Skill->BeginUse();

	//위협 프로파일에 기록
	if (ABattleGameMode* gm = GetWorld()->GetAuthGameMode<ABattleGameMode>())
	{
		gm->RecordSkillUse(ownerCharacter, Skill);
	}

	//커밋 시점까지 대상 생존 불확실, 약참조 스냅샷
	TArray<TWeakObjectPtr<ACharacterBase>> pendingTargets;
	for (ACharacterBase* Target : Targets)
	{
		pendingTargets.Add(Target);
	}

	//대상별 예측 계산·명중 판정·효과 적용은 커밋 시점에 실행
	StartPendingExecution(Skill, [this, Skill, pendingTargets]()
	{
		for (const TWeakObjectPtr<ACharacterBase>& Weak : pendingTargets)
		{
			ACharacterBase* Target = Weak.Get();
			if (!IsValid(Target)) continue;
			RecalculatePending(Skill, Target);
			Target->SetLastAttacker(ownerCharacter);
			if (Target->ReflectDamage() && IsValid(Target))
			{
				Skill->Execute(Target);
			}
		}
	});
}

void USkillComponent::StartPendingExecution(UActiveSkillBase* Skill, TFunction<void()>&& Action)
{
	//이전 pending이 남아있으면 먼저 커밋해 유실 방지
	CommitPendingExecution();

	pendingSkill = Skill;
	pendingAction = MoveTemp(Action);

	float duration = 0.f;
	UAnimMontage* montage = Skill ? Skill->GetCommitMontage() : nullptr;
	if (montage && ownerCharacter)
	{
		pendingMontage = montage;
		duration = ownerCharacter->PlayAnimMontage(montage, Skill->montagePlayRate);
		if (duration > 0.f)
		{
			//노티파이 누락·중단 대비 몽타주 종료 시 커밋 폴백
			if (UAnimInstance* anim = ownerCharacter->GetMesh() ? ownerCharacter->GetMesh()->GetAnimInstance() : nullptr)
			{
				FOnMontageEnded endDelegate;
				endDelegate.BindUObject(this, &USkillComponent::OnSkillMontageEnded);
				anim->Montage_SetEndDelegate(endDelegate, montage);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SkillComponent] 몽타주 재생 실패: %s — ABP의 AnimGraph에 DefaultSlot 노드 필요"),
				*montage->GetName());
		}
	}

	//몽타주가 없거나 재생 실패 시 즉시 커밋해 효과 증발 방지
	if (duration <= 0.f)
	{
		CommitPendingExecution();
	}
}

void USkillComponent::CommitPendingExecution()
{
	if (!pendingAction) return;

	//커밋 액션이 새 pending을 등록할 수 있어 실행 전에 해제
	TFunction<void()> action = MoveTemp(pendingAction);
	pendingAction = nullptr;

	UActiveSkillBase* skill = pendingSkill;
	pendingSkill = nullptr;
	pendingMontage = nullptr;

	//스킬 자체 효과(포복 토글 등) 먼저, 이후 대상별 효과
	if (skill)
	{
		skill->OnCommit();
	}
	action();
}

void USkillComponent::OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//현재 pending 소속이 아닌 몽타주의 종료는 무시 (이전 스킬 몽타주 블렌드아웃 등)
	if (Montage != pendingMontage) return;

	//같은 몽타주 재시작으로 인한 이전 인스턴스 중단이면 새 인스턴스의 종료를 기다림
	if (bInterrupted && ownerCharacter && ownerCharacter->GetCurrentMontage() == Montage) return;

	//노티파이가 이미 커밋했으면 no-op
	CommitPendingExecution();
}

void USkillComponent::SpawnIndicators(UActiveSkillBase* Skill)
{
	if (!ownerCharacter) return;

	UWorld* World = GetWorld();
	if (!World) return;

	const FVector CasterLocation = ownerCharacter->GetActorLocation();

	//시전 가능 범위 인디케이터 생성, 점프는 현재 이동력으로 갈 수 있는 범위로 사거리 원 표시
	//표시 여부도 실효 사거리로 판단, pickRange는 점프에서 상한이 아님
	const float effectivePickRange = Skill->GetEffectivePickRange();
	if (skillRangeIndicatorClass && effectivePickRange > 0.f && Skill->selectMode != ESelectMode::Self)
	{
		skillRangeIndicator = World->SpawnActor<ASkillRangeIndicator>(
			skillRangeIndicatorClass, FTransform(CasterLocation));
		if (skillRangeIndicator)
		{
			skillRangeIndicator->InitRange(effectivePickRange);
		}
	}

	//공격 범위 인디케이터 생성
	if (attackRangeIndicatorClass)
	{
		attackRangeIndicator = World->SpawnActor<AAttackRangeIndicator>(
			attackRangeIndicatorClass, FTransform(CasterLocation));
		if (attackRangeIndicator)
		{
			attackRangeIndicator->InitIndicator(ownerCharacter, Skill);
		}
	}
}

void USkillComponent::DestroyIndicators()
{
	if (IsValid(skillRangeIndicator))
	{
		skillRangeIndicator->Destroy();
		skillRangeIndicator = nullptr;
	}
	if (IsValid(attackRangeIndicator))
	{
		attackRangeIndicator->Destroy();
		attackRangeIndicator = nullptr;
	}
}
