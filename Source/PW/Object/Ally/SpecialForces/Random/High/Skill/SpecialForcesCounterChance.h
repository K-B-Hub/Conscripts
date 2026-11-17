//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesCounterChance.generated.h"

//상급 랜덤 강화, 공격을 피하면 명중과 치명이 오른다
UCLASS()
class PW_API USpecialForcesCounterChance : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesCounterChance();

	virtual void Execute_AfterEvade(ACharacterBase* attacker) override;
};
