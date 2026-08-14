//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Ailment/AilmentBase.h"
#include "Characters/CharacterBase.h"
#include "TimerManager.h"


void UAilmentBase::OnApply(ACharacterBase* affected, ACharacterBase* caster)
{
	//파생 클래스에서 구현
}

void UAilmentBase::Execute(ACharacterBase* affected)
{
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
