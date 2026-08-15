//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "LowMovingPointPassive.generated.h"

//공용 하급 강화, 이동력이 1 증가한다
UCLASS()
class PW_API ULowMovingPointPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	ULowMovingPointPassive();
};
