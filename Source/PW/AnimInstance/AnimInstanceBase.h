// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInstanceBase.generated.h"

class ACharacterBase;
class UStaticMeshComponent;
/**
 * 
 */
UCLASS()
class PW_API UAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UAnimInstanceBase();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(VisibleAnyWhere, BLueprintReadOnly)
	ACharacterBase* OwnerCharacter;

	UPROPERTY(VisibleAnyWhere, BLueprintReadOnly)
	float speed;

	// 감속 부드러움 조절 (클수록 빠르게 반응, 작을수록 부드럽게 감속)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	float InterpSpeed = 8.0f;

	// ─── Hand IK ─────────────────────────────────────────────────
	// Anim Graph의 Two Bone IK 노드가 이 트랜스폼을 목표로 왼손을 정렬함 (컴포넌트 공간)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|IK")
	FTransform LeftHandIKTransform;

	// false일 때 IK 연산 생략 (무기 미장착, 특정 동작 중 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|IK")
	bool bEnableLeftHandIK = false;

	// 무기 메시에 추가할 소켓 이름 (에디터에서 무기 소켓 이름과 맞춰야 함)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|IK")
	FName LeftHandGripSocketName = FName("LeftHandGrip");

private:
	// CharacterBase::WeaponMeshComp 캐시
	// 무기가 교체/제거되면 IsValid()가 false가 되어 자동으로 재탐색됨
	TWeakObjectPtr<UStaticMeshComponent> CachedWeaponMesh;

	void UpdateLeftHandIK();
};
