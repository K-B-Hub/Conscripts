// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SingleAilment.generated.h"

/**
 *
 */
UCLASS()
class PW_API USingleAilment : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USingleAilment();

	virtual void Execute(const ACharacterBase* target) override;
};