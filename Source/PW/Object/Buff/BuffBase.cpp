//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Buff/BuffBase.h"
#include "Characters/CharacterBase.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

void UBuffBase::OnApply(ACharacterBase* affected, ACharacterBase* caster)
{
	//파생 클래스에서 구현
}

void UBuffBase::Execute(ACharacterBase* affected)
{
	//파생 클래스에서 구현
}

void UBuffBase::StartPresentation(ACharacterBase* affected)
{
	if (!affected) return;
	USceneComponent* root = affected->GetRootComponent();

	if (buffEffect)
	{
		effectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(buffEffect, root, NAME_None,
			FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
	}
	if (buffSound)
	{
		soundComponent = UGameplayStatics::SpawnSoundAttached(buffSound, root);
	}
}

void UBuffBase::StopPresentation()
{
	//단발 에셋은 이미 스스로 정리됐을 수 있다, 비활성화하면 남은 입자가 사라진 뒤 자동 파괴
	if (IsValid(effectComponent)) effectComponent->Deactivate();
	if (IsValid(soundComponent)) soundComponent->Stop();
	effectComponent = nullptr;
	soundComponent = nullptr;
}
