//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperVitalAim.generated.h"

//중급 랜덤 강화, 치명이 오른다
UCLASS()
class PW_API USniperVitalAim : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperVitalAim();
};
