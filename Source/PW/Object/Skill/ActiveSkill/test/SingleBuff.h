//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SingleBuff.generated.h"

UCLASS()
class PW_API USingleBuff : public UActiveSkillBase
{
	GENERATED_BODY()
	
public:
	USingleBuff();

	virtual void Execute(const ACharacterBase* target) override;
};
