// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "MultiPick.generated.h"

/**
 * 
 */
UCLASS()
class PW_API UMultiPick : public UActiveSkillBase
{
	GENERATED_BODY()
		
public:
	UMultiPick();
	
	virtual void Execute(const TArray<ACharacterBase*>& targets) override;
};
