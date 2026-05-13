// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/SkillBase.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "Actors/SkillRangeIndicator.h"
#include "Actors/AttackRangeIndicator.h"
#include "Characters/CharacterBase.h"

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

void USkillComponent::AddSkill(USkillBase* Skill)
{
	if (!Skill || skills.Contains(Skill)) return;
	skills.Add(Skill);
	Skill->SetOwner(ownerCharacter);
	UActiveSkillBase* ActiveSkill = Cast<UActiveSkillBase>(Skill);
	if (ActiveSkill)
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