//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Ailment/AilmentBase.h"
#include "Characters/CharacterBase.h"
#include "TimerManager.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"


void UAilmentBase::OnApply(ACharacterBase* affected, ACharacterBase* caster)
{
	//파생 클래스에서 구현
}

void UAilmentBase::Execute(ACharacterBase* affected)
{
}

void UAilmentBase::StartPresentation(ACharacterBase* affected)
{
	if (!affected) return;
	USceneComponent* root = affected->GetRootComponent();

	if (ailmentEffect)
	{
		effectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(ailmentEffect, root, NAME_None,
			FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
	}
	if (ailmentSound)
	{
		soundComponent = UGameplayStatics::SpawnSoundAttached(ailmentSound, root);
	}
}

void UAilmentBase::StopPresentation()
{
	//단발 에셋은 이미 스스로 정리됐을 수 있다, 비활성화하면 남은 입자가 사라진 뒤 자동 파괴
	if (IsValid(effectComponent)) effectComponent->Deactivate();
	if (IsValid(soundComponent)) soundComponent->Stop();
	effectComponent = nullptr;
	soundComponent = nullptr;
}

void UAilmentBase::ScheduleWithPacing(ACharacterBase* affected, TFunction<void(ACharacterBase*)> action)
{
	if (!affected) return;

	if (pacingDelay <= 0.f)
	{
		action(affected);
		return;
	}

	TWeakObjectPtr<UAilmentBase> weakThis(this);
	TWeakObjectPtr<ACharacterBase> weakAffected(affected);
	affected->GetWorldTimerManager().SetTimer(pacingTimerHandle, FTimerDelegate::CreateLambda(
		[weakThis, weakAffected, action]()
		{
			//대기 중 사망·소멸 시 취소, 턴 진행은 사망 처리가 이어받음
			if (!weakThis.IsValid() || !weakAffected.IsValid()) return;

			action(weakAffected.Get());
		}), pacingDelay, false);
}
