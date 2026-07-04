// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "TestDebuffSkill.generated.h"

/**
 * 
 */
UCLASS()
class PW_API UTestDebuffSkill : public UActiveSkillBase
{
	GENERATED_BODY()
	
public:
	UTestDebuffSkill();
	
	virtual void Execute(const ACharacterBase* target) override;
};
