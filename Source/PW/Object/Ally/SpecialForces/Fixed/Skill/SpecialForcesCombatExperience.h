//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesCombatExperience.generated.h"

//레벨 30 고정 강화, 행동력과 장비가 늘어난다
UCLASS()
class PW_API USpecialForcesCombatExperience : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesCombatExperience();
};
