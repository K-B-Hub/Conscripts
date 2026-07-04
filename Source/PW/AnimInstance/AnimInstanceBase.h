// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInstanceBase.generated.h"

class ACharacterBase;
class UStaticMeshComponent;
class USkeletalMeshComponent;
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
	//애니메이션 초기화, 소유 캐릭터와 무기 메시 캐싱
	virtual void NativeInitializeAnimation() override;
	//매 프레임 속도 갱신 및 IK 업데이트
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	ACharacterBase* OwnerCharacter;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	float speed;

	//감속 부드러움 조절
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	float InterpSpeed = 8.0f;

	//TwoBoneIK Effector Location 핀에 직접 연결
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation|IK")
	FVector leftHandIKLocation;

	//false일 때 IK 연산 생략
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|IK")
	bool bEnableLeftHandIK = false;

	//무기 메시에 추가할 소켓 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|IK")
	FName LeftHandGripSocketName = FName("LeftHandGrip");

private:
	//CharacterBase::WeaponMeshComp 캐시, 무기 교체/제거 시 자동 재탐색됨
	TWeakObjectPtr<UStaticMeshComponent> CachedWeaponMesh;

	USkeletalMeshComponent* charMesh = nullptr;

	//왼손 IK 타깃 위치 계산
	void UpdateLeftHandIK();
};
