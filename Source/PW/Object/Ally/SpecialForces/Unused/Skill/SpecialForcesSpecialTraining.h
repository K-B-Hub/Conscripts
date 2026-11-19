//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesSpecialTraining.generated.h"

//미등록(단순 스탯), 회피와 명중이 오르고 장비가 늘어난다
UCLASS()
class PW_API USpecialForcesSpecialTraining : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesSpecialTraining();
};
