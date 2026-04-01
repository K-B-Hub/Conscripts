// Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Skill/ActiveSkillBase.h"
#include "Characters/CharacterBase.h"

bool UActiveSkillBase::CanExecute() const
{
	ACharacterBase* ownerPtr = GetOwner();
	if (!ownerPtr) return false;

	// 행동력 부족 시 사용 불가
	if (ownerPtr->GetCurrentActionPoint() < actionPointCost) return false;

	// 전투 자원 부족 시 사용 불가
	if (ownerPtr->GetBattleResource() < battleResourceCost) return false;

	return true;
}

void UActiveSkillBase::Execute(const TArray<ACharacterBase*>& targets)
{
	// 파생 클래스에서 구현
}
