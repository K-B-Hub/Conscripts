//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "RifleSkilledShooting.generated.h"

//레벨 40 고정 강화, 명중과 치명이 오르고 탄창이 늘어난다
UCLASS()
class PW_API URifleSkilledShooting : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	URifleSkilledShooting();
};
