//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "LowSightPassive.generated.h"

//공용 하급 강화, 시야가 1 증가한다
UCLASS()
class PW_API ULowSightPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	ULowSightPassive();
};
