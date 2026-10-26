//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "RifleCombatExperience.generated.h"

//레벨 30 고정 강화, 행동력과 탄창을 늘린다
UCLASS()
class PW_API URifleCombatExperience : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	URifleCombatExperience();
};
