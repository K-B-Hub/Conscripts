//Fill out your copyright notice in the Description page of Project Settings.

#include "AnimInstance/AnimNotify_Footstep.h"
#include "Characters/CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotify_Footstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	//에디터 프리뷰 등 캐릭터가 아닌 오너는 무시
	ACharacterBase* character = Cast<ACharacterBase>(MeshComp->GetOwner());
	if (!character || !character->GetFootstepSound()) return;

	UGameplayStatics::PlaySoundAtLocation(character, character->GetFootstepSound(), MeshComp->GetComponentLocation());
}
