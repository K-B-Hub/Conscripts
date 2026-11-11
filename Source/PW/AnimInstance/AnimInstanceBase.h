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

	//상체를 잠깐 숙였다 복귀, duration은 전체 왕복 시간
	void PlayBow(float duration);
protected:
	//애니메이션 초기화, 소유 캐릭터와 무기 메시 캐싱
	virtual void NativeInitializeAnimation() override;
	//매 프레임 속도 갱신 및 IK 업데이트
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ACharacterBase* OwnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float speed;

	//점프 궤적 추종 중 여부, 애님BP 공중 스테이트 전이 조건
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|State")
	bool bIsAirborne = false;

	//포복 자세 여부, 애님BP 포복 스테이트 전이 조건
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|State")
	bool bIsProne = false;

	//감속 부드러움 조절
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	float InterpSpeed = 8.0f;

	//조준 지점과 몸통 방향의 차, 에임오프셋 축에 연결 (좌우 +오른쪽, 상하 +위)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Aim")
	float aimYaw = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Aim")
	float aimPitch = 0.f;

	//에임오프셋 보간 속도, 클수록 상체가 빨리 따라감
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|Aim")
	float aimInterpSpeed = 10.f;

	//척추 본 하나에 더할 회전, 애님BP ModifyBone(Component·Additive) Rotation 핀에 연결
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Bow")
	FRotator bowRotation = FRotator::ZeroRotator;

	//숙이는 총 각도, 부호로 앞뒤 방향을 맞춘다
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|Bow")
	float bowAngle = 20.f;

	//숙임을 나눠 받는 척추 본 수, 애님BP의 ModifyBone 노드 수와 맞출 것
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|Bow", meta = (ClampMin = "1"))
	int32 bowBoneCount = 3;

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

	//0이면 숙이기 없음
	float bowTotalTime = 0.f;
	float bowElapsed = 0.f;

	//숙이기 진행도로 bowRotation 갱신
	void UpdateBow(float DeltaTime);
	//왼손 IK 타깃 위치 계산
	void UpdateLeftHandIK();
};
