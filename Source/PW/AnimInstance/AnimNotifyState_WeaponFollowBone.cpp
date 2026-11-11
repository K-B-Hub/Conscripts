//Fill out your copyright notice in the Description page of Project Settings.

#include "AnimInstance/AnimNotifyState_WeaponFollowBone.h"
#include "Characters/CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotifyState_WeaponFollowBone::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	//에디터 프리뷰 등 캐릭터가 아닌 오너는 무시
	ACharacterBase* character = MeshComp ? Cast<ACharacterBase>(MeshComp->GetOwner()) : nullptr;
	if (character) character->SetWeaponFollowBone(boneName);
}

void UAnimNotifyState_WeaponFollowBone::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	//몽타주가 끊겨도 호출되므로 여기서 항상 복귀
	ACharacterBase* character = MeshComp ? Cast<ACharacterBase>(MeshComp->GetOwner()) : nullptr;
	if (character) character->SetWeaponFollowBone(NAME_None);
}
