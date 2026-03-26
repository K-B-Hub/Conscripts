// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/AnimInstanceBase.h"
#include "Characters/CharacterBase.h"
#include "Components/StaticMeshComponent.h"

UAnimInstanceBase::UAnimInstanceBase()
{
}

void UAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ACharacterBase>(GetOwningActor());
}

void UAnimInstanceBase::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (OwnerCharacter)
	{
		const float TargetSpeed = OwnerCharacter->GetVelocity().Size();
		speed = FMath::FInterpTo(speed, TargetSpeed, DeltaTime, InterpSpeed);

		if (bEnableLeftHandIK)
		{
			UpdateLeftHandIK();
		}
	}
}

void UAnimInstanceBase::UpdateLeftHandIK()
{
	// 캐시가 없거나 무기가 제거된 경우 → 캐릭터 자신의 컴포넌트에서 재탐색
	// WeaponMeshComp는 CharacterBase에 직접 붙어있으므로 GetAttachedActors가 아닌
	// FindComponentByClass로 검색
	if (!CachedWeaponMesh.IsValid())
	{
		UStaticMeshComponent* Mesh = OwnerCharacter->FindComponentByClass<UStaticMeshComponent>();
		if (Mesh && Mesh->DoesSocketExist(LeftHandGripSocketName))
		{
			CachedWeaponMesh = Mesh;
		}
	}

	if (!CachedWeaponMesh.IsValid()) return;

	// 소켓 월드 트랜스폼 → 캐릭터 메시 컴포넌트 공간으로 변환
	const FTransform SocketWorld = CachedWeaponMesh->GetSocketTransform(LeftHandGripSocketName);
	const FTransform MeshWorld   = OwnerCharacter->GetMesh()->GetComponentTransform();
	LeftHandIKTransform = SocketWorld.GetRelativeTransform(MeshWorld);
}