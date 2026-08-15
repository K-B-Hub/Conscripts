//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "MiddleMentalityPassive.generated.h"

//공용 중급 강화, 정신력이 2 증가한다
UCLASS()
class PW_API UMiddleMentalityPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UMiddleMentalityPassive();
};
