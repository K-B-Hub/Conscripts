//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TopDamageReductionPassive.generated.h"

//공용 최상급 강화, 받는 피해량 감소 수치가 20 증가한다
UCLASS()
class PW_API UTopDamageReductionPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTopDamageReductionPassive();
};