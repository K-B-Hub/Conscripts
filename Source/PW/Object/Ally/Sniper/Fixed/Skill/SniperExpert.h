//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperExpert.generated.h"

//레벨 70 고정 강화, 주는 피해가 늘어난다
UCLASS()
class PW_API USniperExpert : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperExpert();
};
