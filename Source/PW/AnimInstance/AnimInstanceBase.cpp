// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/AnimInstanceBase.h"
#include "Characters/CharacterBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"

UAnimInstanceBase::UAnimInstanceBase()
{
}

void UAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ACharacterBase>(GetOwningActor());

	UStaticMeshComponent* Mesh = OwnerCharacter->GetWeaponMeshComponent();
	if (Mesh && Mesh->DoesSocketExist(LeftHandGripSocketName))
	{
		CachedWeaponMesh = Mesh;
	}
	charMesh = OwnerCharacter->GetMesh();
}

void UAnimInstanceBase::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (OwnerCharacter)
	{
		const float TargetSpeed = OwnerCharacter->GetVelocity().Size();
		speed = TargetSpeed;

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
		UStaticMeshComponent* Mesh = OwnerCharacter->GetWeaponMeshComponent();
		if (Mesh && Mesh->DoesSocketExist(LeftHandGripSocketName))
		{
			CachedWeaponMesh = Mesh;
		}
	}
	if (charMesh == nullptr)
	{
		charMesh = OwnerCharacter->GetMesh();
	}

	if (!CachedWeaponMesh.IsValid()) return;
	

	// 소켓 월드 위치 → 캐릭터 메시 컴포넌트 공간으로 변환
	const FVector SocketWorldPos = CachedWeaponMesh->GetSocketLocation(LeftHandGripSocketName);
	const FTransform MeshWorld   = charMesh->GetComponentTransform();

	// 스케일 0이거나 미초기화된 트랜스폼이면 역변환 시 NaN 발생
	if (!MeshWorld.IsValid() || MeshWorld.GetScale3D().IsNearlyZero()) return;

	const FVector Result = MeshWorld.InverseTransformPosition(SocketWorldPos);
	if (Result.ContainsNaN()) return;

	leftHandIKLocation = Result;

	// 디버그: IK 타깃 위치를 월드 공간에 구체로 표시 (확인 후 제거)
	DrawDebugSphere(GetWorld(), SocketWorldPos, 5.f, 8, FColor::Green, false, -1.f);
}