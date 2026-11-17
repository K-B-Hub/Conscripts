//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesFragRound.generated.h"

//상급 랜덤 강화, 피해를 준 대상의 방어를 깎는다
UCLASS()
class PW_API USpecialForcesFragRound : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesFragRound();

	virtual void Execute_AfterDamage(ACharacterBase* target) override;
};
