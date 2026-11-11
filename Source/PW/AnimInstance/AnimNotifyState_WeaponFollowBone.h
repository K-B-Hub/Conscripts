//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_WeaponFollowBone.generated.h"

//구간 동안 총을 오른손 대신 애니의 총 본에 붙인다, 재장전처럼 오른손이 총을 놓는 동작용
UCLASS(meta = (DisplayName = "Weapon Follow Bone"))
class PW_API UAnimNotifyState_WeaponFollowBone : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	//팩 애니가 총 위치를 키잉한 본
	UPROPERTY(EditAnywhere, Category = "Weapon")
	FName boneName = FName("ik_hand_gun");
};
