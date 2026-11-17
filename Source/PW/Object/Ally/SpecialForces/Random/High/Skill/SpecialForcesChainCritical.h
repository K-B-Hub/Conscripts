//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesChainCritical.generated.h"

//상급 랜덤 강화, 치명타를 맞히면 치명이 오른다(최대 3중첩)
UCLASS()
class PW_API USpecialForcesChainCritical : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesChainCritical();

	virtual void Execute_AfterCritical(ACharacterBase* target, int32 dealtDamage) override;
};
