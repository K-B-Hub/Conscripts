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

	ownerCharacter = Cast<ACharacterBase>(UActorComponent::GetOwner());		//실제 객체로 재등록
}

// ─── 스킬 추가/제거 ─────────────────────────────────────────────────────────────

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

	// 활성화 중인 스킬이 제거되면 먼저 비활성화
	if (currentSkill == Skill)
	{
		DeactivateSkill();
	}
	skills.Remove(Skill);
}

// ─── 스킬 조회 ──────────────────────────────────────────────────────────────────

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

// ─── 스킬 활성화/비활성화 ───────────────────────────────────────────────────────

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
	currentSkill = nullptr;
}

// ─── 타겟 관리 ──────────────────────────────────────────────────────────────────

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

// ─── 인디케이터 ─────────────────────────────────────────────────────────────────

void USkillComponent::SpawnIndicators(UActiveSkillBase* Skill)
{
	if (!ownerCharacter) return;

	UWorld* World = GetWorld();
	if (!World) return;

	const FVector CasterLocation = ownerCharacter->GetActorLocation();

	// SkillRangeIndicator — Self 모드는 고정 위치이므로 사거리 표시 불필요
	if (skillRangeIndicatorClass && Skill->pickRange > 0.f && Skill->selectMode != ESelectMode::Self)
	{
		skillRangeIndicator = World->SpawnActor<ASkillRangeIndicator>(
			skillRangeIndicatorClass, FTransform(CasterLocation));
		if (skillRangeIndicator)
		{
			skillRangeIndicator->InitRange(Skill->pickRange);
		}
	}

	// AttackRangeIndicator — Skill 객체 직접 전달해 자체 초기화
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