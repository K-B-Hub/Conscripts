//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperGlassCannon.generated.h"

//상급 랜덤 강화, 주는 피해와 받는 피해가 함께 늘어난다
UCLASS()
class PW_API USniperGlassCannon : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperGlassCannon();
};
