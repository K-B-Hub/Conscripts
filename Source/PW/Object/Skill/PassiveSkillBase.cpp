//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkillBase.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "Characters/CharacterBase.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void UPassiveSkillBase::PlayPassiveEffect()
{
	ACharacterBase* character = GetOwner();
	if (!passiveEffect || !character) return;

	UNiagaraFunctionLibrary::SpawnSystemAttached(passiveEffect, character->GetRootComponent(), NAME_None,
		FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
}

void UPassiveSkillBase::SetPassiveEffectActive(bool bActive)
{
	if (bActive)
	{
		if (IsValid(passiveEffectComponent)) return;
		ACharacterBase* character = GetOwner();
		if (!passiveEffect || !character) return;

		//끌 때까지 유지해야 하므로 자동 파괴 안 함
		passiveEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(passiveEffect, character->GetRootComponent(), NAME_None,
			FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
	}
	else
	{
		if (!IsValid(passiveEffectComponent)) return;
		//남은 입자가 사라진 뒤 정리
		passiveEffectComponent->SetAutoDestroy(true);
		passiveEffectComponent->Deactivate();
		passiveEffectComponent = nullptr;
	}
}
