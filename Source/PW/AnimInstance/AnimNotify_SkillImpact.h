//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SkillImpact.generated.h"

//모든 스킬 몽타주 공용 임팩트 노티파이, 해당 프레임에 pending 스킬 효과를 커밋
UCLASS(meta = (DisplayName = "Skill Impact"))
class PW_API UAnimNotify_SkillImpact : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
