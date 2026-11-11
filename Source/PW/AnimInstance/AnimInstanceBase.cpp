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
	if (!OwnerCharacter) return;

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
		//보폭이 메시 스케일만큼 커지므로 원본 크기 기준 속도로 환산해 블렌드스페이스 축을 유지
		const float meshScale = charMesh ? charMesh->GetComponentScale().Z : 1.f;
		speed = meshScale > KINDA_SMALL_NUMBER ? TargetSpeed / meshScale : TargetSpeed;

		bIsAirborne = OwnerCharacter->IsAirborne();
		bIsProne = OwnerCharacter->IsProne();

		//몸통이 아직 못 돈 만큼 상체가 메움, 이동·공중에선 해제
		float targetYaw = 0.f;
		float targetPitch = 0.f;
		FVector aimPoint;
		if (speed < 1.f && !bIsAirborne && OwnerCharacter->GetAimPoint(aimPoint))
		{
			const FRotator toAim = (aimPoint - OwnerCharacter->GetActorLocation()).Rotation();
			const FRotator delta = (toAim - OwnerCharacter->GetActorRotation()).GetNormalized();
			targetYaw = FMath::Clamp(delta.Yaw, -90.f, 90.f);
			targetPitch = FMath::Clamp(delta.Pitch, -45.f, 45.f);
		}
		aimYaw = FMath::FInterpTo(aimYaw, targetYaw, DeltaTime, aimInterpSpeed);
		aimPitch = FMath::FInterpTo(aimPitch, targetPitch, DeltaTime, aimInterpSpeed);

		if (bEnableLeftHandIK)
		{
			UpdateLeftHandIK();
		}
	}

	UpdateBow(DeltaTime);
}

void UAnimInstanceBase::PlayBow(float duration)
{
	bowTotalTime = FMath::Max(duration, 0.1f);
	bowElapsed = 0.f;
}

void UAnimInstanceBase::UpdateBow(float DeltaTime)
{
	if (bowTotalTime <= 0.f) return;

	bowElapsed += DeltaTime;
	const float t = bowElapsed / bowTotalTime;
	if (t >= 1.f)
	{
		bowTotalTime = 0.f;
		bowRotation = FRotator::ZeroRotator;
		return;
	}

	//0→1→0 코사인 곡선, 시작·정점·끝 모두 속도 0이라 튀지 않는다
	const float alpha = 0.5f - 0.5f * FMath::Cos(2.f * PI * t);

	//메시 +X가 캐릭터 왼쪽이라 X축(Roll) 회전이 앞뒤 숙임
	bowRotation = FRotator(0.f, 0.f, bowAngle * alpha / bowBoneCount);
}

void UAnimInstanceBase::UpdateLeftHandIK()
{
	//캐시가 없거나 무기가 제거된 경우 캐릭터 컴포넌트에서 재탐색
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

	//소켓 월드 위치를 캐릭터 메시 컴포넌트 공간으로 변환
	const FVector SocketWorldPos = CachedWeaponMesh->GetSocketLocation(LeftHandGripSocketName);
	const FTransform MeshWorld   = charMesh->GetComponentTransform();

	//스케일 0이거나 미초기화된 트랜스폼이면 역변환 시 NaN 발생
	if (!MeshWorld.IsValid() || MeshWorld.GetScale3D().IsNearlyZero()) return;

	const FVector Result = MeshWorld.InverseTransformPosition(SocketWorldPos);
	if (Result.ContainsNaN()) return;

	leftHandIKLocation = Result;

	//디버그용 IK 타깃 위치 표시
	DrawDebugSphere(GetWorld(), SocketWorldPos, 5.f, 8, FColor::Green, false, -1.f);
}