//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "HighDamageReductionPassive.generated.h"

//공용 상급 강화, 받는 피해량 감소 수치가 6 증가한다
UCLASS()
class PW_API UHighDamageReductionPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UHighDamageReductionPassive();
};
