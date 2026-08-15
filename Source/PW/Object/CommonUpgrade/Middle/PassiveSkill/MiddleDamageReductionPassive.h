//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "MiddleDamageReductionPassive.generated.h"

//공용 중급 강화, 받는 피해량이 3 감소한다
UCLASS()
class PW_API UMiddleDamageReductionPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UMiddleDamageReductionPassive();
};
