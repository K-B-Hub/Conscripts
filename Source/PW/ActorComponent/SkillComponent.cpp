// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/SkillBase.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "Actors/SkillRangeIndicator.h"
#include "Actors/AttackRangeIndicator.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/PassiveSkillComponent.h"

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

	DestroyIndicators();
	ClearCurrentTargets();
	ClearAccumulatedTargets();
	currentSkill = nullptr;

	// 인디케이터 가상 토글이 활성 중이었을 수 있음 — 실제 이동력 차감 여부로 isMoved 원복
	// 이동력이 줄어들었으면 진짜 이동했음(=true), 그대로면 미이동(=false)
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

	// 스킬 인스턴스 멤버를 직접 수정하면 매 호출마다 누적되므로 로컬 카피 후 보정
	// dmg는 damageRatio 소수 절삭 방지 위해 float 유지 — 최종 적용 시 ReceiveDamage가 RoundToInt 수행
	float dmg  = currentSkill->calcDamage;
	int32 amp  = currentSkill->calcDamageAmplfication;
	int32 pen  = currentSkill->calcPenetration;
	float acc  = currentSkill->calcAccuracy;
	float crit = currentSkill->calcCritical;

	// 캐스터 측 BeforeDamageCalc Reactive 패시브 일괄 디스패치 — 조건 충족분만큼 보너스 합산
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
		currentSkill->pickTeam
	);
}

void USkillComponent::SpawnIndicators(UActiveSkillBase* Skill)
{
	if (!ownerCharacter) return;

	UWorld* World = GetWorld();
	if (!World) return;

	const FVector CasterLocation = ownerCharacter->GetActorLocation();

	// SkillRangeIndicator 초기화
	if (skillRangeIndicatorClass && Skill->pickRange > 0.f && Skill->selectMode != ESelectMode::Self)
	{
		skillRangeIndicator = World->SpawnActor<ASkillRangeIndicator>(
			skillRangeIndicatorClass, FTransform(CasterLocation));
		if (skillRangeIndicator)
		{
			skillRangeIndicator->InitRange(Skill->pickRange);
		}
	}

	//AttackRangeIndicator 초기화
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