// Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Skill/SkillBase.h"
#include "Characters/CharacterBase.h"

void USkillBase::SetOwner(ACharacterBase* InOwner)
{
	owner = InOwner;
}

ACharacterBase* USkillBase::GetOwner() const
{
	return owner.Get();
}

void USkillBase::Execute(const TArray<ACharacterBase*>& targets)
{
	// 파생 클래스에서 구현
}
