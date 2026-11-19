//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesCombatMastery.generated.h"

//미등록(단순 스탯), 치명과 치명타 피해가 오른다
UCLASS()
class PW_API USpecialForcesCombatMastery : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesCombatMastery();
};
