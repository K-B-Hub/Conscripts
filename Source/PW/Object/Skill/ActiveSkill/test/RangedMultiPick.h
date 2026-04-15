// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RangedMultiPick.generated.h"

/**
 * 
 */
UCLASS()
class PW_API URangedMultiPick : public UActiveSkillBase
{
	GENERATED_BODY()
	
public:
	URangedMultiPick();
	
	virtual void Execute(const TArray<ACharacterBase*>& targets) override;
};
