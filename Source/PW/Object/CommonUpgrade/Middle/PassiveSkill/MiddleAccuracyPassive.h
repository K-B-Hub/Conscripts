//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "MiddleAccuracyPassive.generated.h"

//공용 중급 강화, 명중이 5 증가한다
UCLASS()
class PW_API UMiddleAccuracyPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UMiddleAccuracyPassive();
};
