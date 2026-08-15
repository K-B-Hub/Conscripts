//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "LowAtkPassive.generated.h"

//공용 하급 강화, 공격력이 1 증가한다
UCLASS()
class PW_API ULowAtkPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	ULowAtkPassive();
};
