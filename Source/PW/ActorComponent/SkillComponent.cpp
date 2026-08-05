//Fill out your copyright notice in the Description page of Project Settings.

#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/SkillBase.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "Actors/SkillRangeIndicator.h"
#include "Actors/AttackRangeIndicator.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/PassiveSkillComponent.h"
#include "GameMode/BattleGameMode.h"

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

	//실제 이동력 차감 여부로 이동 상태 복구
	if (ownerCharacter)
	{
		const bool actuallyMoved = ownerCharacter->GetCurrentMovingPoint() < ownerCharacter->GetMovingPoint();
		ownerCharacter->OnMoveStateChanged(actuallyMoved);
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
	if (!Target || !currentSkill || !ownerCharacter) return;

	//스킬 계산값을 로컬 값으로 복사
	float dmg  = currentSkill->calcDamage;
	int32 amp  = currentSkill->calcDamageAmplfication;
	int32 pen  = currentSkill->calcPenetration;
	float acc  = currentSkill->calcAccuracy;
	float crit = currentSkill->calcCritical;

	//공격자 패시브 보너스 반영
	if (UPassiveSkillComponent* PSC = ownerCharacter->GetPassiveSkillComponent())
	{
		PSC->DispatchBeforeDamageCalc(Target, currentSkill->skillType, currentSkill->damageType,
			dmg, amp, pen, acc, crit);
	}

	Target->CalculateDamage(
		dmg,
		acc,
		crit,
		amp,
		pen,
		currentSkill->skillType,
		ownerCharacter
	);
}

void USkillComponent::DirectExecute(UActiveSkillBase* Skill, const TArray<ACharacterBase*>& Targets)
{
	if (!Skill || Targets.Num() == 0 || !ownerCharacter) return;
	if (!Skill->CanExecute()) return;

	//currentSkill을 임시 지정해 예측 계산 경로 재사용
	UActiveSkillBase* saved = currentSkill;
	currentSkill = Skill;

	//자원 차감은 1회만
	Skill->BeginUse();

	//위협 프로파일에 기록
	if (ABattleGameMode* gm = GetWorld()->GetAuthGameMode<ABattleGameMode>())
	{
		gm->RecordSkillUse(ownerCharacter, Skill);
	}

	//대상별로 예측 계산 후 명중 시 효과 실행
	for (ACharacterBase* Target : Targets)
	{
		if (!IsValid(Target)) continue;
		RecalculatePending(Target);
		Target->SetLastAttacker(ownerCharacter);
		if (Target->ReflectDamage() && IsValid(Target))
		{
			Skill->Execute(Target);
		}
	}

	currentSkill = saved;
}

void USkillComponent::SpawnIndicators(UActiveSkillBase* Skill)
{
	if (!ownerCharacter) return;

	UWorld* World = GetWorld();
	if (!World) return;

	const FVector CasterLocation = ownerCharacter->GetActorLocation();

	//시전 가능 범위 인디케이터 생성, 점프는 현재 이동력으로 갈 수 있는 범위로 사거리 원 표시
	if (skillRangeIndicatorClass && Skill->pickRange > 0.f && Skill->selectMode != ESelectMode::Self)
	{
		skillRangeIndicator = World->SpawnActor<ASkillRangeIndicator>(
			skillRangeIndicatorClass, FTransform(CasterLocation));
		if (skillRangeIndicator)
		{
			skillRangeIndicator->InitRange(Skill->GetEffectivePickRange());
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
